<?
class PXAuthDomain extends PXAuthInterface {
	private $authParams;	

	function PXAuthDomain(&$app, &$request, &$user, &$db, $authParams) {
		$this->authParams = $authParams;
		$this->domain = $this->loadDomain();

		$this->login();

		$uArray = $this->findUser();
		$this->proceedAuth($uArray);
	}

	private function proceedAuth($uArray) {
		if(!count($uArray)) return;
		$this->feelUserFields($uArray);
	}		

	protected function login() {
		if(count($this->domain) != 2
		 || !isset($this->authParams[$this->domain[0]])) {
			return;
		}

		$this->login = (string) _StrToLower($this->domain[1]).'@'.$this->authParams[$this->domain[0]];
	}

	private function loadDomain()  {
		$du = isset($_SERVER['REMOTE_USER']) ? $_SERVER['REMOTE_USER'] : NULL;
		$tmp = explode("\\", $du);
	}
}
?>
