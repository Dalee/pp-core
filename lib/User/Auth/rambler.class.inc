<?php
class PXAuthRambler extends PXAuthInterface {
	
	public function __construct($app, $request, $user, $db, $authParams = null) {
		if (isset($_COOKIE['rsid'])) {
			$this->login();
			$uArray = $this->findUser();
			$this->fillUserFields($uArray);
		}
	}

	function login() {
		$data = $this->getRamblerDataBySession();

		if(isset($data['login'])) 
			$this->login = $data['login'];
	}

	// static
	function getRamblerDataBySession() {
		$checkUrl = 'id.rambler.ru/script/auth.cgi?mode=check&sid='.$_COOKIE['rsid'];
		return PXAuthRambler::_getRamblerData($checkUrl);
	}

	function getRamblerAnswer($params) {
		$url = 'id.rambler.ru/script/auth.cgi?';
		foreach($params as $k=>$v) {
			$url .= $k.'='.$v.'&';
		}
		return PXAuthRambler::_getRamblerData($url);
	}

	function _getRamblerData($checkUrl) {
		$data = @file_get_contents('http://'.$checkUrl);

		if (! mb_strlen($data)) {
			$data = @file_get_contents('http://www.index20.ru/'.$checkUrl);
		}

		if (! mb_strlen($data)) {
			return 0;
		}

		$data = PXAuthRambler::_parseRamblerResponse($data);
		return $data;
	}

	// static
	function _parseRamblerResponse($data) {
		$res = array();
		foreach (explode("\n", $data) as $t) {
			if (! mb_strlen($t)) continue;
			$vv = explode("\t", $t);
			$res[$vv[0]] = $vv[1];
		}
		return $res;
	}
}
?>
