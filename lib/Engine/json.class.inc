<?php
class PXEngineJSON extends PXEngine {
	var $result;

	function __construct(){
		$this->initRequest();
		parent::__construct();
	}

	function initLayout() {
		$this->layout = new PXLayoutNull();
	}

	function initRequest() {
		$this->request = new PXRequestUserJson();
	}

	function runModules() {
		$this->result = $module->userJson();
	}

	function saveResult() {
		$result = $this->result;

		if(!is_array($result)) {
			$result = array();
		}

		$this->request->setResult($result);
	}
}
?>
