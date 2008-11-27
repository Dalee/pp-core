<?
include_once 'null.class.inc';
include_once 'plain.class.inc';
include_once 'secure.class.inc';
include_once 'domain.class.inc';

class PXAuthInterface {
	function PXAuth(&$app, &$request, &$user, &$db, $authParams) {
	}

	function auth() {
		return true;
	}

	function unAuth() {
		return true;
	}

	function encodePasswd($passwd) {
		return $passwd;
	}

	function __parseVar($var) {
		$var = str_replace(' ', '', $var);
		$var = str_replace('/', '', $var);
		$var = str_replace('"', '', $var);
		$var = str_replace("'", '', $var);

		return $var;
	}

	function onAuth(){
		return true;
	}
}
?>