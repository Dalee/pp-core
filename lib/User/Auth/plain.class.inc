<?php
class PXAuthPlain extends PXAuthInterface {
	function PXAuthPlain(&$app, &$request, &$user, &$db) {
		$this->login();
		$this->passwd();
	
		$uArray = $this->findUser();
		$this->proceedAuth($uArray);
	}

	private function proceedAuth($uArray) {
		if(!(count($uArray) && strlen($this->passwd))) {
			return;
		}

		if ($this->passwdToDB($this->passwd) == $uArray['passwd'] || $this->passwd == $this->encodePasswd($uArray['passwd'])) {
			 $this->fillUserFields($uArray);
		}
	}

	function auth() {
		setcookie('login',  $this->login,                       USER_SESSION_INTERVAL, '/', '');
		setcookie('passwd', $this->EncodePasswd($this->passwd), USER_SESSION_INTERVAL, '/', '');
	}

	function unAuth() {
		setcookie('passwd', 0, 0, '/', '');
	}

	function encodePasswd($passwd) {
		return md5('rewqr'.$passwd);
	}
}
?>