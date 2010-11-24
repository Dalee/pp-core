<?php
class PXResponse {
	var $STATUS_OK                      = '200 OK';
	var $STATUS_MOVED                   = '301 Moved Permanently';
	var $STATUS_NOT_MODIFIED            = '304 Not Modified';
	var $STATUS_NOT_FOUND               = '404 Not Found';
	var $STATUS_FORBIDDEN               = '403 Forbidden';
	var $STATUS_NOT_IMPLEMENTED         = '501 Not Implemented';
	var $STATUS_TEMPORARILY_UNAVAILABLE = '503 Service Temporarily Unavailable';

	var $headers = array();
	var $status;
	var $contentType = array();

	function PXResponse() {
		$this->addHeader('X-Powered-By', 'Proxima Portal '.PP_VERSION);

		$this->status = $this->STATUS_OK;
		$this->cache(3600);
		$this->setContentType('text/html', DEFAULT_CHARSET);
	}

	function &getInstance() {
		static $instance;

		if(is_null($instance)) {
			$instance = new PXResponse();
		}

		return $instance;
	}

	function redirect($url, $cacheTimeOut = null) {
		if (ini_get('display_errors') && PXErrorReporter::hasErrors(true)) {
			exit();
		}
		
		// sanitize url
		$url = strtr($url, array(
				' '   => '%20',
				"\r"  => '',
				"\n"  => '',
				"\t"  => '%20',
				'%0D' => '',
				'%0A' => '',
				'%09' => '%20',
				'%00' => ''
			)
		);
		
		$this->status = $this->STATUS_MOVED;

		$this->addHeader('Location', $url);
		
		if (!is_null($cacheTimeOut)) {
			$this->cache($cacheTimeOut);
		}
		else {
			$this->dontCache();
		}
		$this->send();

		exit();
	}

	function addHeader($name, $value) {
		$this->headers[$name] = $value;
	}

	function setOk() {
		$this->status = $this->STATUS_OK;
	}

	function notFound() {
		$this->status = $this->STATUS_NOT_FOUND;
	}
	
	function notModified() {
		$this->status = $this->STATUS_NOT_MODIFIED;
	}
	
	function forbidden() {
		$this->status = $this->STATUS_FORBIDDEN;
	}
	
	function notImplemented() {
		$this->status = $this->STATUS_NOT_IMPLEMENTED;
	}
	
	function unavailable(){
		$this->status = $this->STATUS_TEMPORARILY_UNAVAILABLE;
	}

	function isError() {
		return $this->status != $this->STATUS_OK;
	}

	function dontCache() {
		$this->addHeader('X-Accel-Expires',  0);
		$this->addHeader('Cache-Control', 'no-store, no-cache, must-revalidate, post-check=0, pre-check=0');
		$this->addHeader('Expires', DateUnixToGMT());
	}

	function cache($timeOut = 3600, $xae = null) {
		$this->addHeader('X-Accel-Expires', isset($xae) ? (int)$xae : (int)$timeOut);
		$this->addHeader('Cache-Control', sprintf('private, max-age=%d', (int)$timeOut));
		$this->addHeader('Expires', DateUnixToGMT(time()+(int)$timeOut));
	}

	function setLength($length) {
		$this->addHeader('Content-Length', $length);
	}

	function setCharset($charset) {
		$this->setContentType($this->contentType['type'], $charset);
	}
	
	function setLastModified($timestamp){
		$this->addHeader('Last-Modified', DateUnixToGMT($timestamp));
	}
	
	function setContentType($contentType, $charset=null) {
		$this->contentType['type'] = $contentType;

		if(strlen($charset)) {
			$this->contentType['charset'] = $charset;
		}

		if(!empty($this->contentType['charset'])) {
			$contentType .= ';charset='.$charset;
		}

		$this->addHeader('Content-Type', $contentType);
	}
	
	function downloadFile($filename, $contentType = null, $dispositionType = 'attachment', $charset=null) {
		if (strlen($contentType)) {
			$this->setContentType($contentType, $charset);
		}
		$this->addHeader('Content-Disposition', $dispositionType . '; filename="'. $filename .'"');
	}
	
	function send($content=null) {
		if(!headers_sent()) {
			if($this->isError() || strlen($content)) {
				// FIXME: we really need HTTP/1.1 ?
				// for fix troubles with Transfer-Encoding: chunked
				// use Nginx directive "chunked_transfer_encoding off;"
				// in the http{} or location{} config scopes
				header('HTTP/1.1 '.$this->status);
			}

			if(strlen($content)) {
				$this->setLength(strlen($content));
			}

			$this->_sendHeaders();
		}

		if(strlen($content)) {
			echo $content;
		}
	}

	function _sendHeaders() {
		$sentHeaders = function_exists('apache_response_headers') ? apache_response_headers() : array();
		$notSent = !is_array($sentHeaders);

		foreach($this->headers  as $header => $value) {
			if($notSent || !isset($sentHeaders[$header])) {
				header($header.': '.$value);
			}
		}

		if (function_exists('sendheaders')) {
			sendheaders(); // TODO: remove
		}
	}

	/**
	 * Set cookie
	 *
	 * (Don't send cookie, if headers already sent)
	 *
	 * @param string $name
	 * @param mixed $value
	 * @param integer $expire
	 * @return boolean
	 */
	function SetCookie($name, $value, $expire = NULL) {
		if (headers_sent ()) {
			return false;
		}
		if (is_array($value) || is_object($value)) {
			$value = serialize($value);
		}

		$value = convert_cyr_string($value, 'k', 'w');
		if (!is_numeric($expire)) {
			$expire = 0;
		}
		return setcookie($name, $value, $expire, '/');
	}
}
?>
