<?php
class NLMailMessage {
	var $fromName;
	var $fromEmail;
	var $replyEmail;
	var $toName;
	var $toEmail;
	var $subject;
	var $body;
	var $returnPath;
	var $format;
	var $files;
	var $boundary;
	var $log;
	var $addHeaders;
	var $charset;
 
	var $_errorMessage;
	var $_validateEmails;
	var $_checkMailFields;
 
	function NLMailMessage($logFile=NULL) {
		$this->fromName         = NULL;
		$this->fromEmail        = NULL;
		$this->replyEmail       = NULL;
		$this->toName           = NULL;
		$this->toEmail          = NULL;
		$this->subject          = NULL;
		$this->body             = NULL;
		$this->returnPath       = NULL;
		$this->files            = array();
		$this->format           = 'text';
		$this->multipart        = 'mixed';
		$this->boundary         = strtoupper(uniqid(time()));
		$this->_errorMessage    = NULL;
		$this->log              = $logFile;
		$this->charset          = 'koi8-r';
		$this->_validateEmails  = true;
		$this->_checkMailFields = array('fromEmail', 'replyEmail', 'bccEmail', 'ccEmail', 'toEmail');
		$this->needAttachFiles  = false;
	}
 
	function setCharset($charset) {
		$this->charset = $charset;
	}
 
	function setFrom($name, $email) {
		$this->fromName  = $name;
		$this->fromEmail = $email;
	}
 
	function setTo($name, $email) {
		$this->toName  = strlen($name) ? $name : $email;
		$this->toEmail = $email;
	}
 
	function setCC($email, $hide=true) {
		if($hide) {
			$this->bccEmail = $email;
 
		} else {
			$this->ccEmail = $email;
		}
	}
 
	function setReply($email) {
		$this->replyEmail = $email;
	}
 
	function setSubject($string) {
		$this->subject = $string;
	}

	function setFormat($string) {
		$this->format = $string;
	}

	function setMultipartMixed() {
		$this->multipart = 'mixed';
	}

	function setMultipartRelated() {
		$this->multipart = 'related';
	}

	function setBody($string) {
		$string = str_replace("\xd", '', $string);
		$this->body = $string;
	}
 
	function setTemplate($filename, $withSubject=TRUE, $variables=NULL) {
		if(!is_file($filename)) {
			FatalError('Файл '.$filename.' не найден');
		}
 
		$template = file($filename);
 
		if($withSubject) {
			$subject = trim($template[0]);
 
			$subject = $this->parseTemplate($subject, $variables);
			$this->setSubject($subject);
			array_shift($template);
		}
 
		$template = implode('', $template);
		$template = $this->parseTemplate($template, $variables);
 
		$this->setBody($template);
	}
 
	function parseTemplate($text, $variables) {
		if(!is_array($variables)) {
			return $text;
		}
 
		foreach($variables as $k=>$v) {
			$text = str_replace('%'.strtoupper($k).'%', (string)$v, $text);
		}
 
		$text = preg_replace("/(?<!\%)\%[^%]+?\%/si", '', $text);
		
		$text = str_replace('%%', '%', $text);
		
		return $text;
	}
 
	function setReturnPath($string) {
		$this->returnPath = $string;
	}
 
	function addFile($file, $name, $type='application/octet-stream', $source='', $inline = false) {
		$id = uniqid(rand(), true);
 
		if(isset($this->files[$name])) {
			return $this->files[$name]['id'];
		}
 
		$this->files[$name] = array('file'=>$file, 'name'=>$name, 'id'=>$id, 'type'=>$type, 'source'=>$source, 'inline?' => $inline);
		return 'cid:'.$id;
	}
 
	function createSmtpMail() {
		if (!$this->replyEmail) {
			$this->replyEmail = $this->fromEmail;
		}
 
		$this->_checkEmails();
		$this->_strip();
		$this->_sendImagesAsAttaches();
 
		$this->addHeaders = $this->_writeSmtpHeaders();
 
		$contentType = $this->format == 'html' ? 'text/html' : 'text/plain';
 
		if (sizeof($this->files)) {
			$this->body  = $this->_attachText($this->body, $contentType);
			$this->addHeaders .= $this->_writeMimeHeaders();
 
			foreach ($this->files as $f) {
				$this->body .= $this->_attachFile($f['file'], $f['name'], $f['id'], $f['type'], $f['source'], $f['inline?']);
			}
			$this->body .= "--".$this->boundary."--"."\n";
		} else {
			$this->addHeaders .= "Content-Type: ".$contentType."; charset=".$this->charset."\n";
			$this->addHeaders .= "Content-Transfer-Encoding: 8bit";
		}
 
		$_subject = $this->_getSubject();
 
		return $_subject;
	}
 
	function _getSubject() {
		if (strlen($this->subject)) {
			if (preg_match("/[ю-Ъ]/", $this->subject)) {
				$_subject = "=?".$this->charset."?B?".base64_encode($this->subject)."?=";
			} else {
				$_subject = $this->subject;
			}
		} else {
			$_subject = NULL;
		}
 
		return $_subject;
	}
 
	function _strip() {
		if ($this->format != 'html' and function_exists('strip1251')) {
			$this->fromName = strip1251($this->fromName);
			$this->toName   = strip1251($this->toName);
			$this->subject  = strip1251($this->subject);
			$this->body     = strip1251($this->body);
		}
	}
 
	function createFullSmtpMail() {
		$subject  = $this->createSmtpMail();
 
		$content  = "Subject: $subject\n";
		$content .= $this->addHeaders;
		$content .= "\n\n";
		$content .= $this->body;
 
		return $content;
	}
 
	function send() {
		if (!isset($this->toEmail)) {
			$this->Log(false);
			return false;
		}
 
		$_subject = $this->createSmtpMail();
 
		ini_set('track_errors', 1);
		$oldError = error_reporting(0);
 
		if (!strlen($this->getError()) && @mail($this->toEmail, $_subject, $this->body, $this->addHeaders)) {
			error_reporting($oldError);
 
			$this->log(true);
			return true;
		} else {
			error_reporting($oldError);
			if (!strlen($this->getError())) {
				$this->setError(@$php_errormsg);
			}
 
			$this->log(false);
			return false;
		}
	}
 
	function log($status) {
 
		if(@touch($this->log) && is_file($this->log)) {
			$message = array(
				($status ? 'OK ' : 'ERR'),
				date('r'),
				$this->toEmail,
				$this->fromEmail,
				$this->subject,
				strlen($this->body),
				$this->getError()
			);
 
			appendStringToFile($this->log, implode("\t", $message)."\n");
		}
	}
 
	function setError($string) {
		$this->_errorMessage = $string;
	}
 
	function getError() {
		return $this->_errorMessage;
	}
 
	function _attachText($text, $contentType='text/plain') {
		$out  = "--".$this->boundary."\n";
		$out .= "Content-Type: ".$contentType."; charset=\"".$this->charset."\"\n\n";
		$out .= $text."\n";
		return $out;
	}
 
	function _attachTextAsFile($encoded, $file, $id, $type='application/octet-stream', $inline = false) {
		$out  = "--".$this->boundary . "\n";
		$out .= "Content-type: " . $type . "; name=\"$file\";\n";
		$out .= "Content-Transfer-Encoding: base64\n";
		$out .= "Content-ID: <$id>\n";
		$out .= "Content-disposition: " . ($inline ? 'inline' : 'attachment') . "; filename=\"$file\"\n\n";
		$out .= $encoded . "\n";
		return $out;
	}
 
	function _attachFile($file, $name, $id, $type='application/octet-stream', $source='', $inline = false) {
		if(strlen($source)) {
			$encoded = $this->_encodeString($source);
		} else {
			$encoded = $this->_encodeFile($file);
		}
 
		if (strlen($name)) {
			$file = $name;
		}
 
		return $this->_attachTextAsFile($encoded, $file, $id, $type, $inline);
	}
 
	function _writeSmtpHeaders() {
		$addHeaders = NULL;
 
		if (isset($this->returnPath)) {
			$addHeaders = "Return-Path: <{$this->returnPath}>\n";
		}
 
		if (isset($this->fromName)) {
			$addHeaders .= "From: =?".$this->charset."?B?".base64_encode($this->fromName)."?= <{$this->fromEmail}>\n";
			$addHeaders .= "Reply-To: {$this->replyEmail}\n";
		} else {
			$addHeaders .= "From: {$this->fromEmail}\n";
			$addHeaders .= "Reply-To: {$this->replyEmail}\n";
		}
 
		if (isset($this->ccEmail)) {
			$addHeaders .= "CC: {$this->ccEmail}\n";
		}
 
		if (isset($this->bccEmail)) {
			$addHeaders .= "BCC: {$this->bccEmail}\n";
		}
 
		$addHeaders .= "MIME-Version: 1.0\n";
		return $addHeaders;
	}
 
	function _writeMimeHeaders() {
		$out = "MIME-version: 1.0\n";
		$out .= "Content-type: multipart/{$this->multipart}; ";
		$out .= "boundary=\"$this->boundary\"\n";
		$out .= "Content-transfer-encoding: 7BIT\n";
		return $out;
	}
 
	function _encodeFile($sourcefile) {
		if (is_readable($sourcefile)) {
			return $this->_encodeString(ReadFileToString($sourcefile));
		}
	}
 
	function _encodeString($string) {
		return chunk_split(base64_encode($string));
	}
 
	function _checkEmails() {
		if(!$this->_validateEmails){
			return;
		}
		foreach ($this->_checkMailFields as $field) {
			if (isset($this->$field) && !isEmail($this->$field)) {
				 $this->setError('Задан некорректный e-mail '. $field);
				 break;
			}
		}
	}
 
	function validateEmails($flag) {
		$this->_validateEmails = (bool)$flag;
	}

	function setSendImagesAsAttaches() {
		$this->needAttachFiles = true;
	}

	private
	function _sendImagesAsAttaches() {
		if(!$this->needAttachFiles) {
			return;
		}

		$text = $this->body;
		preg_match_all('#(?:<(img|td|link)[^>]+(src|background|href))="(https?://.+?)"#', $text, $images);

		if(!isset($images[3])) {
			return;
		}

		foreach($images[3] as $i){
			if(strpos($i, '&d=audit') !== false) {
				continue;
			}

			$tmpFile = tempnam('/tmp', 'mailImagesAttach');
			copy($i, $tmpFile);

			$src = $this->addFile($tmpFile, basename($i), mime_content_type($tmpFile));

			$text = str_replace($i, $src, $text);
		}

		$this->body = $text;
	}
}
?>