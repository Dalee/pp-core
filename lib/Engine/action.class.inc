<?php
class PXEngineAction extends PXEngine {
	var $nextLocation;

	function __construct(){
		$this->initRequest();
		parent::__construct();
	}

	function initRequest() {
		$this->request = new PXRequestUser();
	}

	function initLayout() {
		$this->layout = new PXLayoutNull();
	}

	/*function runModules() {
		$response =& PXResponse::getInstance();
		$response->dontCache();

		$this->nextLocation = $module->userAction();
	}*/

	function runModules() {
		$module_name = $this->request->getVar('area');

		$module = PXRegistry::getApp()->modules[$module_name]->getModule();

		$response =& PXResponse::getInstance();
		$response->dontCache();

		$this->nextLocation = $module->userAction();
	}

	function redirect() {
		$nextLocation = $this->nextLocation;

		if (empty($nextLocation)) {
			$nextLocation = $this->request->GetHttpReferer();
		}

		if (empty($nextLocation)) {
			$nextLocation = '/';
		}

		$response =& PXResponse::getInstance();
		$response->redirect($nextLocation);
	}
}
?>
