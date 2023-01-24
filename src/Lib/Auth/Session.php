<?php

namespace PP\Lib\Auth;

use Symfony\Component\HttpFoundation\Session\SessionInterface;

class Session extends AuthAbstract
{
	final public const AUTHORIZED_USER_ID = '__auth_user_id';
	final public const AUTHORIZED_USER_IP = '__auth_user_ip';

	public function isCredentialsValid(array $credentials): bool
	{
		$this->login = getFromArray($credentials, 'login');
		$this->passwd = getFromArray($credentials, 'password');

		$uArray = $this->findUser();

		if ($uArray && strlen((string) $this->passwd) > 0 && static::verifyPassword($this->passwd, $uArray['passwd'])) {
			$this->fillUserFields($uArray);
		}

		return $this->user->id > 0;
	}

	public function isAuthorized(): bool
	{
		// if no session opened, credentials are invalid
		if (!($this->session instanceof SessionInterface)) {
			return false;
		}

		$userId = (int)$this->session->get(static::AUTHORIZED_USER_ID);
		$userIp = (string)$this->session->get(static::AUTHORIZED_USER_IP);

		if ($userId > 0) {
			if ($userIp !== $this->request->GetRemoteAddr()) {
				$this->session->invalidate();
				return false;
			}

			$uArray = $this->db->getObjectById($this->app->types[DT_USER], $userId);

			if (empty($uArray['status'])) {
				$this->session->invalidate();
				return false;
			}

			$this->fillUserFields($uArray);
		}

		return $this->user->id > 0;
	}

	public function auth(): bool
	{
		$this->session->set(static::AUTHORIZED_USER_ID, $this->user->id);
		$this->session->set(static::AUTHORIZED_USER_IP, $this->request->GetRemoteAddr());
		$this->session->migrate(true);
		return true;
	}

	public function unAuth(): bool
	{
		$this->session->invalidate();
		return true;
	}

	public static function passwdToDB(string $passwd): string
	{
		return password_hash($passwd, PASSWORD_BCRYPT);
	}

	public static function verifyPassword(string $plainPassword, string $hash): bool
	{
		return password_verify($plainPassword, $hash);
	}
}
