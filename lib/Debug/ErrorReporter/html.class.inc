<?php

use PP\Lib\Cache\ObjectCache;
use PP\Lib\Http\Response;

class PXErrorReporterHTML extends PXErrorReporter
{

	public function __construct()
	{
		$this->computer = @rtrim((string) `hostname`);

		$this->toEmail = !empty($_SERVER['SERVER_ADMIN']) ? $_SERVER['SERVER_ADMIN'] : 'support@dalee.ru';
		$this->fromEmail = 'robot@' . $this->computer;
	}

	public function warning($error)
	{
		// pass
	}

	public function fatalWarning($error)
	{
		$response = Response::getInstance();
		$host = quot(PXRequest::GetHttpHost());
		$uri = quot(urldecode((string) PXRequest::GetRequestUri()));
		$response->unavailable();
		$response->send(<<<HTML
			<html>
				<head>
					<title>Error</title>
					<style type="text/css">
						body {
							background-color: #ffffff;
							font-family: Lucida Consolas, monospace;
							color: #000000;
						}
					</style>
				</head>

				<body>
					<h1>Error</h1>
					<p>Sorry, but fatal error occured and your request can not be contented</p>
					<p>Please, inform <a href="mailto:{$this->toEmail}?subject=Error%20report%20from%20{$host}&body=%0A%0ARequest%20URI:%20{$uri}">site administrator</a> about it.</p>

					<h1>Ошибка</h1>
					<p>Извините, произошла ошибка, ваш запрос не может быть корректно завершен.</p>
					<p>Пожалуйста, сообщите о ней <a href="mailto:{$this->toEmail}?subject=Сообщение%20об%20ошибке%20с%20сайта%20{$host}&body=%0A%0ARequest%20URI:%20{$uri}">администратору сайта</a>.</p>
				</body>
			</html>
HTML
		);
	}

	public function _throttling()
	{
		try {
			$app = PXRegistry::getApp(); // APP may not be initialized yet and throws exception
			$errorRepDir = $app->getProperty('PP_ERROR_REPORTER_ANTIFLOOD_DIR', 'error_reporter');
			$cacheTime = (int)$app->getProperty('PP_ERROR_REPORTER_ANTIFLOOD_LAG', 30);
		} catch (Exception) {
			$errorRepDir = 'error_reporter';
			$cacheTime = 30;
		}

		if (empty($cacheTime) || empty($errorRepDir)) {
			return [null, false];
		}

		$cache = ObjectCache::get('file', $errorRepDir, $cacheTime);
		$errorSignature = json_encode($this->errorsMap);

		$inLoop = !$cache->expired($errorSignature);
		$curCount = $cache->loadStaled($errorSignature);
		$timer = time();
		$count = $inLoop && $curCount ? [$curCount[0], $curCount[1] + 1] : [$timer, 0];

		$cache->save($errorSignature, $count, $cacheTime - ($timer - $count[0]));
		return [is_array($curCount) ? $curCount[1] : null, $inLoop];
	}

	public function _getMailSubject($cyclesCount)
	{
		$errorTypes = [];
		foreach ($this->errors as $e) {
			@$errorTypes[$e['type']]++;
		}

		uksort($errorTypes, fn($a, $b) => match (true) {
			$a == $b => 0,
			$a == 'Fatal Error' => 1,
			$b == 'Fatal Error' => -1,
			$a == 'Notice' => -1,
			$b == 'Notice' => 1,
			default => 1,
		});

		$subject = [];
		foreach ($errorTypes as $type => $count) {
			$subject[] = $type . '(' . $count . ')';
		}

		return sprintf("%s: %s%s", $this->computer, join(', ', $subject), $cyclesCount ? "; this message throttled $cyclesCount times" : '');
	}

	public function sendReport()
	{
		[$cyclesCount, $inLoop] = $this->_throttling();

		if ($inLoop) {
			return;
		}

		$subject = $this->_getMailSubject($cyclesCount);

		$mail = new NLMailMessage();
		$this->mail =& $mail;

		$mail->validateEmails(false);

		$mail->setFrom('Error Reporter', $this->fromEmail);
		$mail->setSubject($subject);
		$mail->setTo($this->toEmail, $this->toEmail);
		$mail->setFormat('html');
		$mail->setCharset(CHARSET_UTF8);

		$errorsListHTML = [];
		foreach ($this->errors as $errPos => $error) {
			$cls = strtolower(str_replace(' ', '', (string) $error['type']));
			$this->attachError($error, $errPos);

			$message = htmlspecialchars((string) $error['message'], ENT_QUOTES | ENT_DISALLOWED | ENT_SUBSTITUTE, 'UTF-8');
			$errorsListHTML[] = <<<HTML
				<li class="{$cls}">
					<strong>{$error['type']}</strong>:
					{$message}
				</li>
HTML;
		}

		$errorsListHTML = implode('', $errorsListHTML);
		$environmentsUris = $this->environmentsUris();

		$body = <<<HTML
			<html>
				<head>
					<title>{$subject}</title>
					<style type="text/css">
						body { background-color: #ffffff; font-family: Lucida Consolas, monospace; color: #000000; }
						dl { overflow: hidden; }
						dt, dd { float: left; display: block; padding: 0; margin: 0; }
						dt { width: 10em;clear: left; }
						li.fatalerror { color: #ff0000; }
						li.notice     { color: #999999; }
					</style>
				</head>
				<body>
					<ol>{$errorsListHTML}</ol>
					<dl>{$environmentsUris}</dl>
				</body>
			</html>
HTML;

		$this->addEnvironments();
		$mail->setBody($body);
		$mail->send();
	}

	public function environmentsUris()
	{
		$requestUri = PXRequest::GetHttpProto() . '://' . quot(PXRequest::GetHttpHost()) . quot(urldecode((string) PXRequest::GetRequestUri()));
		$remoteAddr = PXRequest::GetRemoteAddr();
		$httpReferer = quot(urldecode((string) PXRequest::GetHttpReferer()));

		return <<<HTML
			<dt>RequestUri:</dt>
			<dd><a target="_blank" href="{$requestUri}">{$requestUri}</a></dd>
			<dt>RemoteAddr:</dt>
			<dd>{$remoteAddr} <a target="_blank" href="https://whatismyipaddress.com/ip/{$remoteAddr}">whois</a></dd>
			<dt>HttpReferer:</dt>
			<dd><a target="_blank" href="{$httpReferer}">{$httpReferer}</a></dd>
HTML;
	}

	public function applyXSLT($xml)
	{
		static $xslt;

		if (is_null($xslt)) {
			$xslt = new PXLibXSLT(__DIR__ . '/style.xsl');
		}

		return $xslt->transform($xml, false);
	}

	public function addEnvironments()
	{
		$xml = ['<?xml version="1.0" encoding="UTF-8"?>'];
		$xml[] = '<?xml-stylesheet type="text/xsl" href="error_reporter.xsl"?>';
		$xml[] = '<environments>';

		$attach = [
			'_POST' => null,
			'_GET' => null,
			'_COOKIE' => null,
			'_SESSION' => null,
			'_SERVER' => null,
			'_FILES' => null,
		];

		foreach ($attach as $k => $v) {
			if (!isset($GLOBALS[$k]) || !is_array($GLOBALS[$k]) || !sizeof($GLOBALS[$k])) {
				continue;
			}

			$xml[] = "\t<{$k}>";

			foreach ($GLOBALS[$k] as $kk => $vv) {
				$node = urldecode($kk);
				$xml[] = "\t\t" . '<' . $node . '><![CDATA[' . urldecode(trim(var_export($vv, true))) . ']]></' . $node . '>';
			}

			$xml[] = "\t</{$k}>";
		}

		$xml[] = '</environments>';

		$html = $this->applyXSLT(implode("\n", $xml));
		$this->mail->addFile(null, 'environments.debug.html', 'text/html', $html);
	}

	public function attachError($error, $errPos)
	{
		if ($errPos < 15) {
			$xml = $this->errorToXML($error);
			$html = $this->applyXSLT($xml);
			$this->mail->addFile(null, 'error_' . ($errPos + 1) . '.debug.html', 'text/html', $html);
		}
	}

	public function errorToXML($error)
	{
		$traceXML = $this->traceToXML($error['trace']);

		$message = htmlspecialchars((string) $error['message'], ENT_XML1 | ENT_QUOTES | ENT_DISALLOWED | ENT_SUBSTITUTE, 'UTF-8');

		return <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<?xml-stylesheet type="text/xsl" href="error_reporter.xsl"?>
<error>
	<type>{$error['type']}</type>
	<code>{$error['code']}</code>
	<file>{$error['file']}</file>
	<line>{$error['line']}</line>
	<message>{$message}</message>
	<trace>{$traceXML}</trace>
</error>
XML;
	}

	public function traceToXML($trace)
	{
		return implode(
			"\n",

			array_map(
				function ($error) {
					$arguments = implode(
						'',

						array_map(
							fn($a) => "\n\t\t\t\t<{$a['type']}>{$a['value']}</{$a['type']}>",
							$error['args']
						)
					);

					return "
		<call>
			<file>{$error['file']}</file>
			<line>{$error['line']}</line>
			<method>{$error['function']}</method>
			<arguments>{$arguments}
			</arguments>
		</call>";
				},

				$trace
			)
		);
	}
}
