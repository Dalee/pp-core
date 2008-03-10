<?
class PXAuthDomain extends PXAuthInterface {
	function PXAuthDomain(&$app, &$request, &$user, &$db, $authParams) {
		$du = isset($_SERVER['REMOTE_USER']) ? $_SERVER['REMOTE_USER'] : NULL;

		$tmp = explode("\\", $du);

		if (count($tmp) != 2) {
			return;
		}

		if (!isset($authParams[$tmp[0]])) {
			return;
		}

		$tmp_login = _StrToLower($tmp[1]).'@'.$authParams[$tmp[0]];
		$tmp = $db->GetObjectsByFieldLimited($app->types[DT_USER], true, 'title', $tmp_login, 1, 0);
		if (!count($tmp)) {
			return;
		}

		$uArray = current($tmp);

		$user->login  = $tmp_login;
		$user->id     = $uArray['id'];
//		$user->passwd = $uArray['passwd'];
		$user->level  = $uArray['access'];
		$user->data   = $uArray;
	}
}
?>