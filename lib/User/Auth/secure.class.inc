<?php
class PXAuthSecure extends PXAuthInterface {

	function PXAuthSecure(&$app, &$request, &$user, &$db) {
		$this->loginAndPasswd();
		$uArray = $this->findUser();
		$this->proceedAuth($uArray);
	}

	private function proceedAuth($uArray) {
		if(!(count($uArray) && strlen($this->passwd))) return;

		if ($this->passwdToDB($this->passwd) == $uArray['passwd'] 
		 || $this->passwd == $this->encodePasswd($uArray['passwd'], false)) {
	 
			 $this->feelUserFields($uArray);
		}
	}

	private function findUser() {
		if(!strlen($this->login)) return null;

		$tmp = PXRegistry::getDB()->GetObjectsByFieldLimited(
			PXRegistry::getApp()->types[DT_USER], true, 'title', $this->login, 1, 0);

		return count($tmp) ? current($tmp) : null;
	}

	private function loginAndPasswd() {
		$request = PXRegistry::getRequest();
		$o = $this;

		return array_map(
			function($field) use ($request, $o) {
				$f = (string) $request->getVar($field);
				$f = call_user_func_array(array($o, "parse".ucfirst($field)), array($f));
	
				if(!strlen($f))
					$f = (string) $request->getCookieVar($field);

				PXRegistry::getUser()->$field = $f;
				return $o->$field = $f;
			}, array('login', 'passwd'));
	}

	private function feelUserFields($uArray) {
		$user = PXRegistry::getUser();

		$user->id     = $uArray['id'];
		$user->level  = $uArray['access'];
		$user->data   = $uArray;

		$this->passwd = $user->passwd = $uArray['passwd'];
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
