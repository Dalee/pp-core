<?php
class PXAuthPlain extends PXAuthInterface {
	
	public function __construct($app, $request, $user, $db, $authParams = null) {
		$this->login();
		$this->passwd();
	
		$uArray = $this->findUser();
		$this->proceedAuth($uArray);
	}

	private function proceedAuth($uArray) {
		if(! (count($uArray) && mb_strlen($this->passwd))) {
			return;
		}

		if (self::passwdToDB($this->passwd) == $uArray['passwd'] || $this->passwd == $this->encodePasswd($uArray['passwd'])) {
			 $this->fillUserFields($uArray);
		}
	}

	function auth() {
		setcookie('login',  $this->login,                       USER_SESSION_INTERVAL, '/', '');
		setcookie('passwd', $this->EncodePasswd($this->passwd), USER_SESSION_INTERVAL, '/', '', false, true); //set passwd HttpOnly
	}

	function unAuth() {
		setcookie('passwd', 0, 0, '/', '');
	}

	function encodePasswd($passwd, $toMd5=true) {
		return md5('rewqr'.$passwd);
	}
}
?>
