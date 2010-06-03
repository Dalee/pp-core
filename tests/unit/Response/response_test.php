<?php

class ResponseTest extends UnitTestCase {
	function setUp() {
		$this->r = PXResponse::getInstance();
	}
	
	function testGetInstance() {
		$this->assertIsA(PXResponse::getInstance(), "PXResponse");
	}
	
	function testStatuses() {
		$statuses_map = array(
			"STATUS_OK"              => '200 OK',
			"STATUS_MOVED"           => '301 Moved Permanetly',
			"STATUS_NOT_FOUND"       => '404 Not found',
			"STATUS_FORBIDDEN"       => '403 Forbidden',
			"STATUS_NOT_IMPLEMENTED" => '501');
		
		$vars = get_object_vars($this->r);
		
		foreach($statuses_map as $k=>$v) {
			if(!isset($vars[$k])) continue;
			$this->assertEqual($vars[$k], $v);
		}
	}
	
	/*function addHeader($name, $value) {
		$this->headers[$name] = $value;
	}*/
	function testAddHeader() {
		$this->r->addHeader("TEST", "BLABLA");
		$this->assertEqual($this->r->headers["TEST"], "BLABLA");
	}

	/*function setOk() {
		$this->status = $this->STATUS_OK;
	}*/
	function testSetOk() {
		$this->r->setOk();
		$this->assertEqual($this->r->status, "200 OK");
	}	

	/*function notFound() {
		$this->status = $this->STATUS_NOT_FOUND;
	}*/
	function testNotFound() {
		$this->r->notFound();
		$this->assertEqual($this->r->status, "404 Not found");
	}

	/*function forbidden() {
		$this->status = $this->STATUS_FORBIDDEN;
	}*/
	function testForbidden() {
		$this->r->forbidden();
		$this->assertEqual($this->r->status, "403 Forbidden");
	}
		
	/*function notImplemented() {
		$this->status = $this->STATUS_NOT_IMPLEMENTED;
	}*/
	function testNotImplemented() {
		$this->r->notImplemented();
		$this->assertEqual($this->r->status, "501");
	}
	
	
	/*function isError() {
		return !($this->status == $this->STATUS_OK);
	}*/
	function testIsError() {
		$this->r->setOk();
		$this->assertFalse($this->r->isError());
		
		$this->r->forbidden();
		$this->assertTrue($this->r->isError());
	}

	/*function dontCache() {
		$this->addHeader('X-Accel-Expires',  0);
		$this->addHeader('Cache-Control', 'no-cache, must-revalidate');
	}*/
	function testDontCache() {
		$this->r->dontCache();
		$hdrs = $this->r->headers;
		
		$this->assertEqual($hdrs['X-Accel-Expires'], 0);
		$this->assertEqual($hdrs['Cache-Control'], 'no-cache, must-revalidate');
	}
	
	/*function cache($timeOut = 3600) {
		$this->addHeader('X-Accel-Expires', $timeOut);
		$this->addHeader('Cache-Control', 'public');
	}*/
	function testCache() {
		/*$l = function ($timeout, $o) {
				$hdrs = $o->r->headers;
				$o->assertEqual($hdrs['X-Accel-Expires'], $timeout);
				$o->assertEqual($hdrs['Cache-Control'], 'public');
		};

		$this->r->cache();
		$l(3600, $this);
		
		$this->r->cache(20000);
		$l(20000, $this); FIXME: php 5.3.x ONLY !*/
	}
	
	/*function setLength($length) {
		$this->addHeader('Content-Length', $length);
	}*/
	function testSetLength() {
		$this->r->setLength(100);
		$this->assertEqual($this->r->headers['Content-Length'], 100);
	}
	
	/*function setCharset($charset) {
		$this->setContentType($this->contentType['type'], $charset);
	}*/
	function testSetCharset() {
		$this->r->setCharset("test-charset");
		$this->assertEqual("test-charset", $this->r->contentType['charset']);
		$this->assertPattern("#^.*;charset=test-charset$#", $this->r->headers['Content-Type']);
	}
		
	/*function setContentType($contentType, $charset=null) {
		$this->contentType['type'] = $contentType;

		if(strlen($charset)) {
			$this->contentType['charset'] = $charset;
		}

		if(strlen($charset)) {
			$contentType .= ';charset='.$charset;
		}

		$this->addHeader('Content-Type', $contentType);
	}*/
	function testSetContentType() {
		$this->r->setContentType("test-type", "test-charset");
		$this->assertEqual($this->r->contentType['charset'], "test-charset");
		$this->assertEqual($this->r->headers['Content-Type'], "test-type;charset=test-charset");
	}

	/*function downloadFile($filename, $contentType = null, $dispositionType = 'attachment', $charset=null) {
		if (strlen($contentType)) {
			$this->setContentType($contentType, $charset);
		}
		$this->addHeader('Content-Disposition', $dispositionType . '; filename="'. $filename .'"');
	}*/
	function testDownloadFile() {
		$this->r->downloadFile("test.test", "test-type");
		$this->assertEqual($this->r->headers['Content-Disposition'], 'attachment; filename="test.test"');
	}
	
	/*function send($content=null) {
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
	}*/
	function testSend() {
		//...
	}

	/*function _sendHeaders() {
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
	}*/
	function test_sendHeaders() {
		//...
	}

}

?>
