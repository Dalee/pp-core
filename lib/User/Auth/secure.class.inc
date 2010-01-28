<?php
class PXAuthSecure extends PXAuthInterface {
	function PXAuthSecure(&$app, &$request, &$user, &$db) {
		$this->login();
		$this->passwd();

		$uArray = $this->findUser();
		$this->proceedAuth($uArray);
	}

	private function proceedAuth($uArray) {
		if(!(count($uArray) && strlen($this->passwd))) {
			return;
		}

		if ($this->passwdToDB($this->passwd) == $uArray['passwd'] || $this->passwd == $this->encodePasswd($uArray['passwd'], false)) {
			 $this->feelUserFields($uArray);
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
	
	function passwdToDB($passwd){
		return md5($passwd);
	}

	function auth() {
		setcookie('login',  $this->login,                              USER_SESSION_INTERVAL, '/', '');
		setcookie('passwd', $this->encodePasswd($this->passwd, false), USER_SESSION_INTERVAL, '/', '');
	}

	function unAuth() {
		setcookie('passwd', 0, 0, '/', '');
	}
}
?>
