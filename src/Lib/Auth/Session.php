<?php

namespace PP\Lib\Auth;

use Symfony\Component\HttpFoundation\Session\Session as SystemSession;

class Session extends AuthAbstract {

	const AUTHORIZED_USER_ID = '__auth_user_id';
	const AUTHORIZED_USER_IP = '__auth_user_ip';

	public function isCredentialsValid() {
		// if no session opened, credentials are invalid
		if (!($this->session instanceof SystemSession)) {
			return false;
		}

		$userId = (int)$this->session->get(static::AUTHORIZED_USER_ID);
		$userIp = (string)$this->session->get(static::AUTHORIZED_USER_IP);
		if ($userId > 0) {
			if ($userIp !== $this->request->GetRemoteAddr()) {
				return false;
			}

			$uArray = $this->db->getObjectById($this->app->types[DT_USER], $userId);
			$this->fillUserFields($uArray);

			return true;
		}

		// adminAction request
		$this->login = $this->request->getPostVar('login');
		$this->passwd = $this->request->getPostVar('passwd');

		$uArray = $this->findUser();
		if ((strlen($this->passwd) > 0) && (static::passwdToDB($this->passwd) == $uArray['passwd'])) {
			$this->fillUserFields($uArray);
		}

		return $this->user->id !== null;
	}

	function auth() {
		$this->session->set(static::AUTHORIZED_USER_ID, $this->user->id);
		$this->session->set(static::AUTHORIZED_USER_IP, $this->request->GetRemoteAddr());
		$this->session->migrate(true);
		return true;
	}

	function unAuth() {
		$this->session->set(static::AUTHORIZED_USER_ID, 0);
		$this->session->set(static::AUTHORIZED_USER_IP, '');
		return true;
	}

	public static function passwdToDB($passwd) {
		return md5($passwd);
	}
}
