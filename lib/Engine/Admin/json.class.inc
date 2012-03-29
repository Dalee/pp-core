<?php
class PXEngineAdminJSON extends PXEngineJSON {
	function __construct() {
		parent::__construct();
	}

	function initModules() {
		$this->area    = $this->request->getArea();
		$this->modules = PXEngineAdmin::getModule($this->app, $this->area);
		$this->checkArea($this->area);
	}

	function getJson($module) {
		return $module->adminJson();
	}

	public function engineClass(){
		return PX_ENGINE_ADMIN;
	}
}
?>
