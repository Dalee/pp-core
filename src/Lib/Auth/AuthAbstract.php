<?php

namespace PP\Lib\Auth;

use PXApplication;
use PXDatabase;
use PXRegistry;
use PXRequest;
use PXUser;
use Symfony\Component\HttpFoundation\Session\SessionInterface;

abstract class AuthAbstract implements AuthInterface
{

	/** @var PXRequest */
	protected $request;

	/** @var PXDatabase */
	protected $db;

	/** @var PXApplication */
	protected $app;

	/** @var PXUser */
	protected $user;

	/** @var null|SessionInterface */
	protected $session;

	/** @var @var string|null */
	protected $login;

	/** @var @var string|null */
	protected $passwd;

	public function __construct(?array $params = [])
	{
		// params is not used right now..
	}

	/**
	 * {@inheritdoc}
	 */
	public function setRequest(PXRequest $request): AuthInterface
	{
		$this->request = $request;

		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	public function setDb(PXDatabase $db): AuthInterface
	{
		$this->db = $db;

		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	public function setApp(PXApplication $app): AuthInterface
	{
		$this->app = $app;

		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	public function setUser(PXUser $user): AuthInterface
	{
		$this->user = $user;

		return $this;
	}

	/**
	 * @param null|SessionInterface $session
	 * @return $this
	 */
	public function setSession(?SessionInterface $session = null): AuthInterface
	{
		$this->session = $session;

		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	abstract public function isCredentialsValid(array $credentials): bool;

	/**
	 * {@inheritdoc}
	 */
	abstract public function isAuthorized(): bool;

	public function fillUserFields(array $uArray): void
	{
		$user = $this->user ?: PXRegistry::getUser();

		$user->id = $uArray['id'] ?? null;
		$user->login = $uArray['title'] ?? null;
		$user->data = $uArray;
		$this->passwd = $user->passwd = $uArray['passwd'] ?? null;
	}

	public function getTitle(): ?string
	{
		return $this->user->login;
	}

	protected function findUser(): ?array
	{
		if (!mb_strlen($this->login)) {
			return null;
		}

		$tmp = $this->db->getObjectsByFieldLimited(
			$this->app->types[DT_USER],
			true,
			'title',
			$this->login,
			1,
			0
		);

		return count($tmp) ? current($tmp) : null;
	}

	public function auth(): bool
	{
		return true;
	}

	public function unAuth(): bool
	{
		return true;
	}

	/**
	 * Метод-триггер, вызывается в PXUser::checkAuth() после загрузки правил acl,
	 * позволяет выполнить дополнительные проверки.
	 *
	 * @return bool
	 */
	public function onAuth(): bool
	{
		return true;
	}

	public static function passwdToDB(string $passwd): string
	{
		return $passwd;
	}

	public static function verifyPassword(string $plainPassword, string $hash): bool
	{
		return static::passwdToDB($plainPassword) === $hash;
	}
}
