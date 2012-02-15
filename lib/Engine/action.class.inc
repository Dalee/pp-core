<?php
class PXEngineAction extends PXEngine {
	var $nextLocation;

	function __construct(){
		parent::profilerStart();
		parent::__construct();
	}

	function runModules() {
		$module = $this->modules[$this->area]->getModule();

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