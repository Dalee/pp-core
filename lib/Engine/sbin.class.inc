<?php
class PXEngineSbin extends PXEngine {
	var $result;

	function __construct(){
		parent::__construct();

		$this->initApplication();

		$this->initRequest();
		$this->initUser();
		$this->initLayout();

		$this->saveToRegistry();
	}

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

	function initModules() {
	}

	function runModules() {
	}
}
?>