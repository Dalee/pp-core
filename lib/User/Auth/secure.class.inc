<?php
class PXAuthSecure extends PXAuthInterface {

	public function __construct(&$app, &$request, &$user, &$db, $authParams = null) {
		$this->login();
		$this->passwd();

		$uArray = $this->findUser();
		$this->_proceedAuth($uArray);
	}

	protected function _proceedAuth($uArray) {
		if(!isset($uArray['passwd']) || !strlen($this->passwd)) {
			return;
		}

		if (self::passwdToDB($this->passwd) == $uArray['passwd'] || $this->passwd == $this->encodePasswd($uArray['passwd'], false)) {
			$this->fillUserFields($uArray);
		}
	}

	function encodePasswd($passwd, $toMd5=true) {
		$md = $toMd5 ? md5($passwd) : $passwd;
		$tmp = '';

		for($i=0; $i<strlen($md);$i++) {
			$tmp .= dechex(hexdec($md{$i}) ^ $i);
		}

		return $tmp;
	}

	public static function passwdToDB($passwd) {
		return md5($passwd);
	}

	function auth() {
		setcookie('login',  $this->login,                              USER_SESSION_INTERVAL, '/', '');
		setcookie('passwd', $this->encodePasswd($this->passwd, false), USER_SESSION_INTERVAL, '/', '', false, true); //set passwd HttpOnly
	}

	function unAuth() {
		setcookie('passwd', 0, 0, '/', '');
	}
}
