<?php

namespace PP\Lib\Http;

use Exception;
use PXErrorReporter;
use PXRegistry;
use Symfony\Component\HttpFoundation\Cookie;
use Symfony\Component\HttpFoundation\JsonResponse;
use Symfony\Component\HttpFoundation\RedirectResponse;
use Symfony\Component\HttpFoundation\Response as HttpResponse;
use Symfony\Component\HttpFoundation\ResponseHeaderBag;
use Symfony\Component\HttpFoundation\StreamedResponse;

final class Response
{
	private static ?\PP\Lib\Http\Response $instance = null;

	/** @var HttpResponse */
	private readonly HttpResponse $httpResponse;

	/**
	 * Response constructor.
	 */
	public function __construct()
	{
		$this->httpResponse = new HttpResponse();

		try {
			$app = PXRegistry::getApp(); // APP may not be initialized yet and throws exception
			$charset = $app->getProperty('OUTPUT_CHARSET', DEFAULT_CHARSET);
			$cacheTime = $app->getProperty('PP_RESPONSE_CACHE_EXPIRATION', 3600);

		} catch (Exception) {
			$charset = DEFAULT_CHARSET;
			$cacheTime = 3600;
		}

		$this->cache($cacheTime);
		$this->setContentType('text/html', $charset);
	}

	/**
	 * @param int $timeOut
	 * @param null $xae
	 * @return $this
	 */
	public function cache($timeOut = 3600, $xae = null): self
	{
		$this->httpResponse
			->setPrivate()
			->setMaxAge((int)$timeOut);

		$this->addHeader('X-Accel-Expires', intval($xae ?? $timeOut));

		return $this;
	}

	/**
  * @return $this
  */
 public function addHeader(string $name, string $value): self
	{
		$this->httpResponse->headers->set($name, $value);
		return $this;
	}

	/**
  * @return $this
  */
 public function setContentType(string $contentType, ?string $charset = null): self
	{
		$this->httpResponse->headers->set('Content-Type', $contentType);

		if (mb_strlen($charset)) {
			$this->httpResponse->setCharset($charset);
		}

		return $this;
	}

	public static function getInstance(): self
	{
		if (is_null(static::$instance)) {
			static::$instance = new self();
		}

		return static::$instance;
	}

	/**
	* @param string|int|null $cacheTimeOut
	*/
	public function redirect(string $url, $cacheTimeOut = null, int $statusCode = HttpResponse::HTTP_MOVED_PERMANENTLY): void
	{
		if (ini_get('display_errors') && PXErrorReporter::hasErrors()) {
			exit();
		}

		if (!is_null($cacheTimeOut)) {
			$this->cache($cacheTimeOut);
		} else {
			$this->dontCache();
		}

		$redirectResponse = new RedirectResponse($url, $statusCode, $this->httpResponse->headers->all());

		$redirectResponse->send();

		exit();
	}

	/**
	 * @return $this
	 */
	public function dontCache(): self
	{
		$this->httpResponse->setPublic();
		$this->addHeader('X-Accel-Expires', 0)
			->addHeader('Cache-Control', 'no-cache, no-store, must-revalidate')
			->addHeader('Expires', DateUnixToGMT());

		return $this;
	}

	public function send(?string $content = null): void
	{
		if (mb_strlen($content)) {
			// NOTICE: no mb_ here
			$this->setLength(strlen($content));
		}
		$this->httpResponse->setContent($content);
		$this->httpResponse->send();
	}

	/**
	* @return $this
	*/
	public function setLength(int $length): self {
		$this->addHeader('Content-Length', $length);
		return $this;
	}

	/**
	 * @return $this
	 */
	public function setOk(): self
	{
		$this->httpResponse->setStatusCode(HttpResponse::HTTP_OK);
		return $this;
	}

	/**
	 * @return $this
	 */
	public function notFound(): self
	{
		$this->httpResponse->setStatusCode(HttpResponse::HTTP_NOT_FOUND);
		return $this;
	}

	// Cache management

	/**
	 * @return $this
	 */
	public function forbidden(): self
	{
		$this->httpResponse->setStatusCode(HttpResponse::HTTP_FORBIDDEN);
		return $this;
	}

	/**
	 * @return $this
	 */
	public function unavailable(): self
	{
		$this->httpResponse->setStatusCode(HttpResponse::HTTP_SERVICE_UNAVAILABLE);
		$this->noCache();
		return $this;
	}

	/**
	 * @return $this
	 */
	public function noCache(): self
	{ // not sure about caching behaviour, make upstream server responsible for this
		$this->removeHeader('X-Accel-Expires')
			->removeHeader('Cache-Control')
			->removeHeader('Expires');

		return $this;
	}

	/**
	* @return $this
	*/
	public function removeHeader(string $name): self
	{
		$this->httpResponse->headers->remove($name);
		return $this;
	}

	public function isError(): bool
	{
		return $this->httpResponse->isClientError() || $this->httpResponse->isServerError();
	}

	/**
	* @return $this
	*/
	public function setStatus(int $code): self
	{
		$this->httpResponse->setStatusCode($code);
		return $this;
	}

	/**
	* @return $this
	*/
	public function setCharset(string $charset): self
	{
		$this->httpResponse->setCharset($charset);
		return $this;
	}

	/**
	* @return $this
	*/
	public function downloadFile(string $filename, ?string $contentType = null,
								 string $dispositionType = ResponseHeaderBag::DISPOSITION_ATTACHMENT,
								 ?string $charset = null): self
	{
		if (mb_strlen($contentType)) {
			$this->setContentType($contentType, $charset);
		}

		$disposition = $this->httpResponse->headers->makeDisposition($dispositionType, $filename);

		$this->httpResponse->headers->set('Content-Disposition', $disposition);

		return $this;
	}

	/**
	* @param null $jsonData
	*/
	public function sendJson($jsonData = null, ?string $contentType = null, ?string $charset = null): void
	{
		if (mb_strlen($contentType)) {
			$this->setContentType($contentType, $charset);
		} else {
			$this->httpResponse->headers->remove('Content-Type'); // JsonResponse class automatic set proper content type
		}

		// TODO: add JsonResponse callback support?
		$jsonResponse = new JsonResponse($jsonData, $this->httpResponse->getStatusCode(), $this->httpResponse->headers->all());
		$jsonResponse->headers->set('Content-Length', strlen($jsonResponse->getContent()));
		$jsonResponse->send();
	}

	public function sendStream(callable $callback): void
	{
		$streamedResponse = new StreamedResponse($callback, $this->httpResponse->getStatusCode(), $this->httpResponse->headers->all());
		$streamedResponse->send();
	}

	/**
	 * Set cookie
	 *
	 * (Don't send cookie, if headers already sent)
	 *
	 * @param string $name
	 * @param mixed $value
	 * @param int|null $expire
	 * @param string|null $domain
	 * @param string $path
	 * @param bool $secure
	 * @param bool $httpOnly
	 * @param bool $raw
	 * @param string|null $sameSite
	 * @return bool
	 * @noinspection PhpTooManyParametersInspection
	 */
	public function setCookie(string $name, mixed $value, ?int $expire = null, ?string $domain = null, string $path = '/',
							  bool $secure = false, bool $httpOnly = false, bool $raw = false, ?string $sameSite = null)
	{
		if (is_array($value) || is_object($value)) {
			$value = serialize($value);
		}

		if (!is_numeric($expire)) {
			$expire = 0;
		}

		$this->httpResponse->headers->setCookie(new Cookie($name, $value, $expire, $path, $domain, $secure,
			$httpOnly, $raw, $sameSite));

		return true;
	}

	/**
	* Get access to Symfony Response instance for edge cases
	*/
	public function getBaseResponse(): HttpResponse
	{
		return $this->httpResponse;
	}
}
