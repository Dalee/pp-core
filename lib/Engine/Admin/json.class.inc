<?php
class PXEngineAdminJSON extends PXEngineJSON {
	function __construct(){
		parent::__construct();
	}
	
	function initModules() {
	}
	
	function getJson($module) {
		return $module->adminJson();
	}

	public function engineClass(){
		return PX_ENGINE_ADMIN;
	}
}
?>
