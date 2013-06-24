<?php
class NLHTTPClient {
	var $connection;
	var $timeout;
	var $connectionTimeout;
	var $redirects;
	var $headers;
	var $status;

	var $host;
	var $port;
	var $path;

	var $replyHeaders;
	var $replyBody;

	var $proxy;

	function NLHTTPClient($proxy=NULL, $timeout=10, $connectionTimeout = array(10, 0)) {
		if(is_null($proxy)) {
			$this->proxy = NULL;

		} else {
			$tmp = parse_url($proxy);

			$this->proxy['host'] = isset($tmp['host']) && strlen($tmp['host']) ? $tmp['host'] : '';
			$this->proxy['port'] = isset($tmp['port']) && $tmp['port'] > 0     ? $tmp['port'] : '80';
			$this->proxy['user'] = isset($tmp['user']) && strlen($tmp['user']) ? $tmp['user'] : 'anonymous';
			$this->proxy['pass'] = isset($tmp['pass']) && strlen($tmp['pass']) ? $tmp['pass'] : '';
		}

		$this->timeout           = $timeout;
		$this->connectionTimeout = is_array($connectionTimeout) ? $connectionTimeout : array((int)$connectionTimeout, 0);
		$this->connection        = false;
		$this->headers           = array();
		$this->redirects         = 0;
		$this->status            = 0;
	}

	function _connect($host='', $port='80') {
		if(!$this->connection || (!is_array($this->proxy) && ($host != $this->host || $port != $this->port))) {
			if(is_array($this->proxy)) {
				$host = $this->proxy['host'];
				$port = $this->proxy['port'];

				if($this->proxy['user'] != 'anonymous' || strlen($this->proxy['pass'])) {
					$this->headers[] = "Proxy-Authorization: Basic " . base64_encode ($this->proxy['user'].":".$this->proxy['pass']);
				}
			}

			$this->headers[] = 'Connection: close';

			$this->host = $host;
			$this->port = $port;

			if($this->connection) {
				$this->CloseConnect();
			}

			$fp = fsockopen($host, $port, $errno, $errstr, (int)$this->timeout);

			if (!$fp) {
				$this->connection = false;
				FatalError($errstr);
			}
			
			$this->connection = $fp;
		}

		return $this->connection;
	}


	function Get($url=NULL, $auth=false, $fullPath=false) {
		$tmp = parse_url($url);

		unset($this->headers);
		unset($this->replyHeaders);
		unset($this->replyBody);

		$host    = isset($tmp['host'])  && strlen($tmp['host'])  ? $tmp['host'] : '';
		$port    = isset($tmp['port'])  && $tmp['port'] > 0      ? $tmp['port'] : '80';
		$path    = isset($tmp["path"])  && strlen($tmp["path"])  ? $tmp["path"] : '/';
		$path   .= isset($tmp["query"]) && strlen($tmp["query"]) ? '?'.$tmp["query"] : '';

		$this->headers[] = "GET ".($fullPath ? 'http://'.$host: '').$path." HTTP/1.0";
		$this->headers[] = "Host: ".$host;

		if(is_string($auth) && strlen($auth)) {
			$user = isset($tmp['user']) && strlen($tmp['user']) ? $tmp['user'] : 'anonymous';
			$pass = isset($tmp['pass']) && strlen($tmp['pass']) ? $tmp['pass'] : '';

			switch(strtolower($auth)) {
				case 'basic':
					$this->headers[] = 'Authorization: Basic '.base64_encode($user.':'.$pass);
					break;

				case 'digest':
					$this->headers[] = 'Authorization: Digest '.md5($user.':'.$pass);
					break;
			}
		}

		$this->_connect($host, $port);

		if($this->connection) {
			fwrite($this->connection, implode("\r\n", $this->headers)."\r\n\r\n");
			stream_set_timeout($this->connection, $this->connectionTimeout[0], $this->connectionTimeout[1]);
			
			$tmp = '';
			while (!getFromArray(stream_get_meta_data($this->connection), 'timed_out') && !feof($this->connection)) {
				$tmp .= fread($this->connection, 4096);
			}
			
			$this->CloseConnect();
			
			$this->reply        = $tmp;
			$this->replyHeaders = explode("\r\n", substr($this->reply, 0, strpos($this->reply,"\r\n\r\n")));
			$this->replyBody    = substr($this->reply, strpos($this->reply,"\r\n\r\n")+4);

			switch($this->GetLastStatus()) {
				case '200':
					$this->redirects = 0;
					$result = $this->replyBody;
					break;

				case '301':
				case '302':
				case '303':
					if($this->redirects < 50) {
						$this->redirects++;
						$result = $this->Get($this->FindLocation(), $auth, $fullPath);

					} else {
//						FatalError('Too many redirects');
						$result = false;
					}
					break;

				case '400':
					FatalError($this->replyBody);
					break;

				case '401':
					if($this->redirects < 3) {
						$this->redirects++;
						$result = $this->Get($url, $this->FindAuthorizationType(), $fullPath);

					} else {
//						FatalError('Too many redirects');
						$result = false;
					}
					break;

				default:
				case '404':
					$result = false;
					break;
			}


		} else {
			$result = false;
		}

		return $result;
	}

	function GetLastStatus() {
		if(isset($this->replyHeaders[0]) && strlen($this->replyHeaders[0])) {
			preg_match('|^HTTP/1\\.\\d (\\d{3}) |', $this->replyHeaders[0], $tmp);
			$this->status = $tmp[1];

		} else {
			$this->status = 404;
		}

		return $this->status;
	}

	function FindAuthorizationType() {
		foreach($this->replyHeaders as $h) {
			preg_match('#^WWW-Authenticate:\\s?(BASIC|DIGEST).*?$#i', $h, $tmp);

			if(isset($tmp[1]) && strlen($tmp[1])) {
				return $tmp[1];
			}
		}

		return false;
	}

	function FindLocation() {
		foreach($this->replyHeaders as $h) {
			preg_match('|^Location:\\s?(.*?)$|i', $h, $tmp);

			if(isset($tmp[1]) && strlen($tmp[1])) {
				if(strpos($tmp[1], 'http://') === 0 || strpos($tmp[1], 'https://') === 0) {
					return $tmp[1];
				} else {
					return 'http://'.$this->host.$tmp[1];
				}
			}
		}

		return false;
	}

	function CloseConnect() {
		$tmp = @!!fclose($this->connection);
		$this->connection = false;
		return $tmp;
	}
}
?>
