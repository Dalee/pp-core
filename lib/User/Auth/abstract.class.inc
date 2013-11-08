<?php
class PXAuthInterface {
	public $login;
	public $passwd;

	public function __construct($app, $request, $user, $db, $authParams = null) {
	}

	protected function fillUserFields($uArray) {
		$user = PXRegistry::getUser();

		$user->id     = $uArray['id'];
		$user->data   = $uArray;

		$this->passwd = $user->passwd = $uArray['passwd'];
	}

	function getTitle() {
		return PXRegistry::getUser()->login;
	}

	protected function findUser() {
		if (! mb_strlen($this->login)) {
			return null;
		}

		$tmp = PXRegistry::getDB()->GetObjectsByFieldLimited(PXRegistry::getApp()->types[DT_USER], true, 'title', $this->login, 1, 0);

		return count($tmp) ? current($tmp) : null;
	}

	protected function _lazySetAuthField($field) {
		$request = PXRegistry::getRequest();

		$f = $request->getVar($field);
		$f = call_user_func_array(array($this, "parse".ucfirst($field)), array($f));

		if ((! is_string($f)) || (! mb_strlen($f))) {
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

	function encodePasswd($passwd, $toMd5=true) {
		return $passwd;
	}

	public static function passwdToDB($passwd) {
		return $passwd;
	}

	function parseLogin($login){
		return is_string($login) ? preg_replace('/[^\w\.\@\-]/'.REGEX_MOD, '', mb_substr($login, 0, 255)) : null;
	}

	function parsePasswd($password){
		return is_string($password) ? $password : null;
	}

	protected function login(){
		$this->_lazySetAuthField("login");
	}

	protected function passwd(){
		$this->_lazySetAuthField("passwd");
	}
}
?>
