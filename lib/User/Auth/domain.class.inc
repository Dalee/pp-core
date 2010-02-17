<?php
class PXAuthDomain extends PXAuthInterface {
	private $authParams;	

	function PXAuthDomain(&$app, &$request, &$user, &$db, $authParams) {
		$this->authParams = $authParams;
		$this->domain = $this->_loadDomain();

		$this->_login();

		$uArray = $this->findUser();
		$this->_proceedAuth($uArray);
	}

	private function _proceedAuth($uArray) {
		if(!count($uArray)) return;
		$this->fillUserFields($uArray);
	}		

	protected function _login() {
		if(count($this->domain) != 2
		 || !isset($this->authParams[$this->domain[0]])) {
			return;
		}

		$this->login = (string) _StrToLower($this->domain[1]).'@'.$this->authParams[$this->domain[0]];
	}

	private function _loadDomain()  {
		$du = isset($_SERVER['REMOTE_USER']) ? $_SERVER['REMOTE_USER'] : NULL;
		return explode("\\", $du);
	}
}
?>