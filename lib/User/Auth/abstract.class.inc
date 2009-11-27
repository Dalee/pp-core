<?php
class PXAuthInterface {
	function PXAuth(&$app, &$request, &$user, &$db, $authParams) {
	}

	protected function feelUserFields($uArray) {
		$user = PXRegistry::getUser();

		$user->id     = $uArray['id'];
		$user->data   = $uArray;

		$this->passwd = $user->passwd = $uArray['passwd'];
	}

	protected function findUser() {
		if(!strlen($this->login)) {
			return null;
		}

		$tmp = PXRegistry::getDB()->GetObjectsByFieldLimited(PXRegistry::getApp()->types[DT_USER], true, 'title', $this->login, 1, 0);

		return count($tmp) ? current($tmp) : null;
	}

	protected function lazySetAuthField($field) {
		$request = PXRegistry::getRequest();

		$f = $request->getVar($field);
		$f = call_user_func_array(array($this, "parse".ucfirst($field)), array($f));

		if(!strlen($f) || !is_string($f)) {
			$f = (string) $request->getCookieVar($field);
		}

		PXRegistry::getUser()->$field = $f;
		return $this->$field = $f;
	}

	function auth() {
		return true;
	}

	function unAuth() {
		return true;
	}

	/**
	 * Метод-триггер, вызывается в PXUser::checkAuth() после загрузки правил acl, 
	 * позволяет выполнить дополнительные проверки.
	 *
	 * @return bool
	 */
	function onAuth(){
		return true;
	}

	function encodePasswd($passwd) {
		return $passwd;
	}

	function passwdToDB($passwd) {
		return $passwd;
	}

	function parseLogin($login){
		if(!is_string($login)) {
			return;
		}

		return preg_replace('/[^\w\.\@\-]/', '', substr($login, 0, 255));
	}

	function parsePasswd($password){
		if(!is_string($password)) {
			return;
		}

		return preg_replace('/[^\w\.\@\,\-]/', '', substr($password, 0, 255));
	}

	protected function login(){
		$this->lazySetAuthField("login");
	}

	protected function passwd(){
		$this->lazySetAuthField("passwd");
	}
}
?>
