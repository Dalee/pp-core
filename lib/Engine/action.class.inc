<?php
class PXEngineAction extends PXEngine {
	var $nextLocation;

	function initLayout() {
		$this->layout = new PXLayoutNull();
	}

	function runModules() {
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

		Finalize($nextLocation);
	}
}
?>
