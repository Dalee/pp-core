<?php
class PXEngineSbin extends PXEngine {
	var $result;

	function __construct(){
		$this->initApplication();

		$this->initRequest();
		$this->initUser();
		$this->initLayout();


		$this->saveToRegistry();

		//parent::__construct();
	}

	function initLayout() {
		$this->layout = new PXLayoutNull();
	}

	function initRequest() {
		$this->request = new PXRequestNull();
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
