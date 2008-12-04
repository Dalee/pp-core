<?php
class PXAuthSecure extends PXAuthInterface {
	function PXAuthSecure(&$app, &$request, &$user, &$db) {
		$login   = $this->login($request);
		$passwd  = $this->passwd($request);

		$cPasswd = null;

		if(is_null($login) || is_null($passwd) || !is_string($passwd)) {
			$login   = $request->getCookieVar('login');
			$cPasswd = $request->getCookieVar('passwd');
			$passwd = null;
		}

		$login   = $this->__parseVar($login);
		$passwd  = $this->__parseVar($passwd);

		if(strlen($login) > 32) {
			$login = substr($login, 0, 32);
		}

		$this->login  = $user->login = $login;

		if (is_null($login) || (is_null($passwd) && is_null($cPasswd))) {
			return;
		}

		$tmp = $db->GetObjectsByFieldLimited($app->types[DT_USER], true, 'title', $login, 1, 0);
		if (!count($tmp)) {
			return;
		}

		$uArray = current($tmp);
		if (!strlen($uArray['passwd'])) {
			return;
		}

		if (md5($passwd) != $uArray['passwd'] && $cPasswd != $this->encodePasswd($uArray['passwd'], false)) {
			return;
		}

		$user->id     = $uArray['id'];
		$user->login  = $login;

		$this->passwd = $user->passwd = $uArray['passwd'];

		$user->level  = $uArray['access'];
		$user->data   = $uArray;

		return $this;
	}

	function encodePasswd($passwd, $toMd5=true) {
		$md = $toMd5 ? md5($passwd) : $passwd;
		$tmp = '';
		for($i=0; $i<strlen($md);$i++) {
			$tmp .= dechex(hexdec($md{$i}) ^ $i);
		}

		return $tmp;
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