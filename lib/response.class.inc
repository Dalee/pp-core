<?php
class PXResponse {
	var $STATUS_OK              = '200 OK';
	var $STATUS_MOVED           = '301 Moved Permanetly'; // òÅËÏÍÅÎÄÁÃÉÑ áÛÍÁÎÏ×Á É Co
	var $STATUS_NOT_FOUND       = '404 Not found';
	var $STATUS_FORBIDDEN       = '403 Forbidden';
	var $STATUS_NOT_IMPLEMENTED = '501';

	var $headers = array();
	var $status;
	var $contentType = array();

	function PXResponse() {
		$this->addHeader('X-Powered-By', 'Proxima Portal '.PP_VERSION);

		$this->status = $this->STATUS_OK;
		$this->cache(3600);
		$this->setContentType('text/html', 'koi8-r');
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
	
	function forbidden() {
		$this->status = $this->STATUS_FORBIDDEN;
	}
	
	function notImplemented() {
		$this->status = $this->STATUS_NOT_IMPLEMENTED;
	}

	function isError() {
		return !($this->status == $this->STATUS_OK);
	}

	function dontCache() {
		$this->addHeader('X-Accel-Expires',  0);
		$this->addHeader('Cache-Control', 'no-cache, must-revalidate');
	}

	function cache($timeOut = 3600) {
		$this->addHeader('X-Accel-Expires', $timeOut);
		$this->addHeader('Cache-Control', 'public');
	}

	function setLength($length) {
		$this->addHeader('Content-Length', $length);
	}

	function setCharset($charset) {
		$this->setContentType($this->contentType['type'], $charset);
	}
	
	function setContentType($contentType, $charset=null) {
		$this->contentType['type'] = $contentType;

		if(strlen($charset)) {
			$this->contentType['charset'] = $charset;
		}

		if(strlen($charset)) {
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
			header('HTTP/1.1 '.$this->status);

			if(is_string($content)) {
				$this->setLength(strlen($content));
			}

			$this->_sendHeaders();
		}

		if(is_string($content)) {
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
}
?>
