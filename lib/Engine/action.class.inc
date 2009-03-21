<?php
class PXEngineAction extends PXEngine {
	var $nextLocation;

	function initLayout() {
		$this->layout = new PXLayoutNull();
	}

	function runModules() {
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
