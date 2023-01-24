<?php

class NLMailMessage
{
    public $fromName;
    public $fromEmail;
    public $replyEmail;
    public $toName;
    public $toEmail;
    public $subject;
    public $body;
    public $returnPath;
    public $format;
    public $files;
    public $boundary;
    public $addHeaders;
    public $charset;

    public $_errorMessage;
    public $_validateEmails;
    public $_checkMailFields;

    public $_listHeadersFields;
    public $listHeaders;

    public function __construct(public $log = NULL)
    {
        $this->fromName = NULL;
        $this->fromEmail = NULL;
        $this->replyEmail = NULL;
        $this->toName = NULL;
        $this->toEmail = NULL;
        $this->subject = NULL;
        $this->body = NULL;
        $this->returnPath = NULL;
        $this->files = [];
        $this->format = 'text';
        $this->multipart = 'mixed';
        $this->boundary = mb_strtoupper(uniqid(time()));
        $this->_errorMessage = NULL;
        $this->charset = DEFAULT_CHARSET;
        $this->_validateEmails = true;
        $this->_checkMailFields = ['fromEmail', 'replyEmail', 'bccEmail', 'ccEmail', 'toEmail'];
        $this->needAttachFiles = false;
        $this->_listHeadersFields = ['help', 'subscribe', 'unsubscribe', 'owner', 'archive'];
        $this->listHeaders = [];
    }

    public function setCharset($charset)
    {
        $this->charset = $charset;
    }

    public function setFrom($name, $email)
    {
        $this->fromName = $name;
        $this->fromEmail = $email;
    }

    public function setTo($name, $email)
    {
        $this->toName = mb_strlen((string) $name) ? $name : $email;
        $this->toEmail = $email;
    }

    public function setCC($email, $hide = true)
    {
        if ($hide) {
            $this->bccEmail = $email;

        } else {
            $this->ccEmail = $email;
        }
    }

    public function setReply($email)
    {
        $this->replyEmail = $email;
    }

    public function setSubject($string)
    {
        $this->subject = $string;
    }

    public function setFormat($string)
    {
        $this->format = $string;
    }

    public function setMultipartMixed()
    {
        $this->multipart = 'mixed';
    }

    public function setMultipartRelated()
    {
        $this->multipart = 'related';
    }

    public function setListHeaders($list, $variables = null)
    {
        $this->listHeaders = array_intersect_key((array)$list, array_flip($this->_listHeadersFields));
        foreach ($this->listHeaders as $k => $v) {
            $this->listHeaders[$k] = $this->parseTemplate($v, $variables);
        }
    }

    public function setBody($string)
    {
        $string = str_replace("\xd", '', (string) $string);
        $this->body = $string;
    }

    public function setTemplate($filename, $withSubject = TRUE, $variables = NULL)
    {
        if (!is_file($filename)) {
            FatalError('Файл ' . $filename . ' не найден');
        }

        $template = file($filename);

        if ($withSubject) {
            $subject = trim($template[0]);

            $subject = $this->parseTemplate($subject, $variables);
            $this->setSubject($subject);
            array_shift($template);
        }

        $template = implode('', $template);
        $template = $this->parseTemplate($template, $variables);

        $this->setBody($template);
    }

    public function parseTemplate($text, $variables)
    {
        if (!is_array($variables)) {
            return $text;
        }

        foreach ($variables as $k => $v) {
            if (is_scalar($v)) {
                $text = str_replace('%' . mb_strtoupper($k) . '%', (string)$v, (string) $text);
            }
        }

        $text = preg_replace("/(?<!\%)\%[^%]+?\%/si" . REGEX_MOD, '', (string) $text);

        $text = str_replace('%%', '%', $text);

        return $text;
    }

    public function setReturnPath($string)
    {
        $this->returnPath = $string;
    }

    public function addFile($file, $name, $type = 'application/octet-stream', $source = '', $inline = false)
    {
        $id = uniqid(random_int(0, mt_getrandmax()), true);

        if (isset($this->files[$name])) {
            return 'cid:' . $this->files[$name]['id'];
        }

        $this->files[$name] = ['file' => $file, 'name' => $name, 'id' => $id, 'type' => $type, 'source' => $source, 'inline?' => $inline];
        return 'cid:' . $id;
    }

    public function createSmtpMail()
    {
        if (!$this->replyEmail) {
            $this->replyEmail = $this->fromEmail;
        }

        $this->_checkEmails();
        $this->_strip();
        $this->_sendImagesAsAttaches();

        $this->addHeaders = $this->_writeSmtpHeaders();

        $contentType = $this->format == 'html' ? 'text/html' : 'text/plain';

        if (sizeof($this->files)) {
            $this->body = $this->_attachText($this->body, $contentType);
            $this->addHeaders .= $this->_writeMimeHeaders();

            foreach ($this->files as $f) {
                $this->body .= $this->_attachFile($f['file'], $f['name'], $f['id'], $f['type'], $f['source'], $f['inline?']);
            }
            $this->body .= "--" . $this->boundary . "--" . "\n";
        } else {
            $this->addHeaders .= "Content-Type: " . $contentType . "; charset=" . $this->charset . "\n";
            $this->addHeaders .= "Content-Transfer-Encoding: 8bit";
        }

		return $this->_getSubject();
    }

    public function _getSubject()
    {
        if (mb_strlen((string) $this->subject)) {
            $_subject = "=?" . $this->charset . "?B?" . base64_encode((string) $this->subject) . "?=";
        } else {
            $_subject = NULL;
        }

        return $_subject;
    }

    public function _strip()
    {
        if ($this->format != 'html' and function_exists('strip1251')) {
            $this->fromName = strip1251($this->fromName);
            $this->toName = strip1251($this->toName);
            $this->subject = strip1251($this->subject);
            $this->body = strip1251($this->body);
        }
    }

    public function createFullSmtpMail()
    {
        $subject = $this->createSmtpMail();

        $content = "Subject: $subject\n";
        $content .= $this->addHeaders;
        $content .= "\n\n";
        $content .= $this->body;

        return $content;
    }

    public function send()
    {
        $php_errormsg = null;
        if (!isset($this->toEmail)) {
            $this->Log(false);
            return false;
        }

        $_subject = $this->createSmtpMail();

        ini_set('track_errors', 1);
        $oldError = error_reporting(0);

        if (!mb_strlen((string) $this->getError()) && @mail((string) $this->toEmail, (string) $_subject, (string) $this->body, $this->addHeaders)) {
            error_reporting($oldError);

            $this->log(true);
            return true;
        } else {
            error_reporting($oldError);
            if (!mb_strlen((string) $this->getError())) {
                $this->setError(@$php_errormsg);
            }

            $this->log(false);
            return false;
        }
    }

    public function log($status)
    {

        if (@touch($this->log) && is_file($this->log)) {
            $message = [
                ($status ? 'OK ' : 'ERR'),
                date('r'),
                $this->toEmail,
                $this->fromEmail,
                $this->subject,
                mb_strlen((string) $this->body),
                $this->getError(),
            ];

            appendStringToFile($this->log, implode("\t", $message) . "\n");
        }
    }

    public function setError($string)
    {
        $this->_errorMessage = $string;
    }

    public function getError()
    {
        return $this->_errorMessage;
    }

    public function _attachText($text, $contentType = 'text/plain')
    {
        $out = "--" . $this->boundary . "\n";
        $out .= "Content-Type: " . $contentType . "; charset=\"" . $this->charset . "\"\n\n";
        $out .= $text . "\n";
        return $out;
    }

    public function _attachTextAsFile($encoded, $file, $id, $type = 'application/octet-stream', $inline = false)
    {
        $out = "--" . $this->boundary . "\n";
        $out .= "Content-type: " . $type . "; name=\"$file\";\n";
        $out .= "Content-Transfer-Encoding: base64\n";
        $out .= "Content-ID: <$id>\n";
        $out .= "Content-disposition: " . ($inline ? 'inline' : 'attachment') . "; filename=\"$file\"\n\n";
        $out .= $encoded . "\n";
        return $out;
    }

    public function _attachFile($file, $name, $id, $type = 'application/octet-stream', $source = '', $inline = false)
    {
        if (mb_strlen((string) $source)) {
            $encoded = $this->_encodeString($source);
        } else {
            $encoded = $this->_encodeFile($file);
        }

        if (mb_strlen((string) $name)) {
            $file = $name;
        }

        return $this->_attachTextAsFile($encoded, $file, $id, $type, $inline);
    }

    public function _writeSmtpHeaders()
    {
        $addHeaders = NULL;

        if (isset($this->returnPath)) {
            $addHeaders = "Return-Path: <{$this->returnPath}>\n";
        }

        if (isset($this->fromName)) {
            $addHeaders .= "From: =?" . $this->charset . "?B?" . base64_encode((string) $this->fromName) . "?= <{$this->fromEmail}>\n";
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

        if (!empty($this->listHeaders)) {
            foreach ($this->listHeaders as $type => $values) {
                if (empty($values) || !in_array(mb_strtolower((string) $type), ['help', 'subscribe', 'unsubscribe', 'owner', 'archive'])) {
                    continue;
                }
                $values = "<" . join('>, <', (array)$values) . ">";
                $type = mb_ucfirst(mb_strtolower((string) $type));
                $addHeaders .= "List-{$type}: {$values}\n";
            }
        }

        $addHeaders .= "MIME-Version: 1.0\n";
        return $addHeaders;
    }

    public function _writeMimeHeaders()
    {
        $out = "MIME-version: 1.0\n";
        $out .= "Content-type: multipart/{$this->multipart}; ";
        $out .= "boundary=\"$this->boundary\"\n";
        $out .= "Content-transfer-encoding: 7BIT\n";
        return $out;
    }

    public function _encodeFile($sourcefile)
    {
        if (is_readable($sourcefile)) {
            return $this->_encodeString(ReadFileToString($sourcefile));
        }
    }

    public function _encodeString($string)
    {
        return chunk_split(base64_encode((string) $string));
    }

    public function _checkEmails()
    {
        if (!$this->_validateEmails) {
            return;
        }
        foreach ($this->_checkMailFields as $field) {
            if (isset($this->$field) && !isEmail($this->$field)) {
                $this->setError('Задан некорректный e-mail ' . $field);
                break;
            }
        }
    }

    public function validateEmails($flag)
    {
        $this->_validateEmails = (bool)$flag;
    }

    public function setSendImagesAsAttaches()
    {
        $this->needAttachFiles = true;
    }

    private
    function _sendImagesAsAttaches()
    {
        if (!$this->needAttachFiles) {
            return;
        }

        $text = $this->body;
        preg_match_all('#(?:<(img|td|link)[^>]+(src|background|href))="(https?://.+?)"#' . REGEX_MOD, (string) $text, $images);

        if (!isset($images[3])) {
            return;
        }

        foreach ($images[3] as $i) {
            if (mb_strpos((string) $i, '&d=audit') !== false) {
                continue;
            }

            $tmpFile = tempnam('/tmp', 'mailImagesAttach');
            copy($i, $tmpFile);

            $src = $this->addFile($tmpFile, basename((string) $i), mime_content_type($tmpFile));

            $text = str_replace($i, $src, (string) $text);
        }

        $this->body = $text;
    }
}
