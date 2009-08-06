<?php
class PXEngineJSON extends PXEngine {
	var $result;

	function __construct(){
		$this->initRequest();
		$this->initLayout();

		$this->saveToRegistry();
		parent::__construct();
	}

	function initLayout() {
		$this->layout = new PXLayoutNull();
	}

	function initRequest() {
		$this->request = new PXRequestUserJson();
	}

	function runModules() {
		$module_name = $this->request->getVar('area');

		$module = PXRegistry::getApp()->modules[$module_name]->getModule();

		$response =& PXResponse::getInstance();
		$response->dontCache();

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
