<?php

namespace PP\Lib\Auth;

class Secure extends AuthAbstract {

	public function isCredentialsValid() {
		$this->login();
		$this->passwd();

		$uArray = $this->findUser();
		$this->_proceedAuth($uArray);

		return !is_null($this->user->id);
	}

	protected function _proceedAuth($uArray) {
		if ((!isset($uArray['passwd'])) || (!mb_strlen($this->passwd))) {
			return;
		}

		$result = self::passwdToDB($this->passwd) == $uArray['passwd'];
		$result = $result || $this->passwd == $this->encodePasswd($uArray['passwd'], false);

		if ($result) {
			$this->fillUserFields($uArray);
		}
	}

	protected function encodePasswd($passwd, $toMd5 = true) {
		$md = $toMd5 ? md5($passwd) : $passwd;
		$tmp = '';

		for ($i = 0; $i < mb_strlen($md); $i++) {
			$tmp .= dechex(hexdec(mb_substr($md, $i, 1)) ^ $i);
		}

		return $tmp;
	}

	public static function passwdToDB($passwd) {
		return md5($passwd);
	}

	public function auth() {
		setcookie('login',
			$this->login,
			USER_SESSION_INTERVAL,
			'/',
			''
		);

		setcookie('passwd',
			$this->encodePasswd($this->passwd, false),
			USER_SESSION_INTERVAL,
			'/',
			'',
			false,
			true
		);
	}

	public function unAuth() {
		setcookie('passwd', 0, 0, '/', '');
	}
}
