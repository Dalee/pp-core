<?php
class PXEngineSbin extends PXEngine {
	var $result;

	function initLayout() {
		$this->layout = new PXLayoutNull();
	}

	function initRequest() {
		$this->request = new PXNullRequest();
	}

	function initUser() {
		$this->user = new PXUserCron();
	}

	function initDirectories() {
	}

	function runModules() {
	}
}
?>
