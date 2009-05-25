<?
class PXAuthInterface {
	function PXAuth(&$app, &$request, &$user, &$db, $authParams) {
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
		return preg_replace('/[^\w\.\@\-]/', '', substr($login, 0, 255));
	}
	
	function parsePasswd($password){
		return preg_replace('/[^\w\.\@\,\-]/', '', substr($password, 0, 255));
	}
	
	function login(&$getFromRequest){
		return $getFromRequest->GetVar('login');
	}
	
	function passwd(&$getFromRequest){
		return $getFromRequest->GetVar('passwd');
	}
}
?>