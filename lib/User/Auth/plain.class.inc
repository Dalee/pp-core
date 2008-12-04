<?php
class PXAuthPlain extends PXAuthInterface {
	function PXAuthPlain(&$app, &$request, &$user, &$db) {
		$login  = $this->login($request);
		$passwd = $this->passwd($request);

		if(is_null($login) || is_null($passwd)) {
			$login  = $request->GetCookieVar('login');
			$passwd = $request->GetCookieVar('passwd');
		}

		$login   = $this->__parseVar($login);
		$passwd  = $this->__parseVar($passwd);

		if(strlen($login) > 32) {
			$login = substr($login, 0, 32);
		}

		$this->login  = $user->login = $login;
		$this->passwd = $passwd;

		if (is_null($login) || is_null($passwd)) {
			return;
		}

		$tmp = $db->getObjectsByFieldLimited($app->types[DT_USER], true, 'title', $login, 1, 0);
		if (!count($tmp)) {
			return;
		}

		$uArray = current($tmp);
		if (!strlen($uArray['passwd'])) {
			return;
		}

		if ($passwd != $uArray['passwd'] && $passwd != $this->EncodePasswd($uArray['passwd'])) {
			return;
		}

		$user->id     = $uArray['id'];
		$user->login  = $login;
//		$user->passwd = $uArray['passwd'];
		$user->level  = $uArray['access'];
		$user->data   = $uArray;
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