<?

class PXAuthRambler extends PXAuthInterface {
	function PXAuthRambler(&$app, &$request, &$user, &$db) {
		if (!isset($_COOKIE['rsid'])) {
			return;
		}

		$data = $this->getRamblerDataBySession();
		$tmp = $db->getObjectsByFieldLimited($app->types[DT_USER], true, 'title', $data['login'], 1, 0);

		if (!count($tmp)) {
			return ;
		}
		$tmp = current($tmp);

		$user->id     = $tmp['id'];
		$user->login  = $tmp['title'];
		$user->level  = $tmp['access'];
		$user->data   = $tmp;
		
	}

	// static
	function getRamblerDataBySession() {
		$checkUrl = 'id.rambler.ru/script/auth.cgi?mode=check&sid='.$_COOKIE['rsid'];
		$data = @file_get_contents('http://'.$checkUrl);

		if (!strlen($data)) {
			$data = @file_get_contents('http://www.index20.ru/'.$checkUrl);
		}
		
		if (!strlen($data)) {
			return 0;
		}

		$data = PXAuthRambler::_parseRamblerResponse($data);
		return $data;
	}

	// static
	function _parseRamblerResponse($data) {
		$res = array();
		foreach (explode("\n", $data) as $t) {
			if (!strlen($t)) continue;
			$vv = explode("\t", $t);
			$res[$vv[0]] = $vv[1];
		}
		return $res;
	}

}

?>
