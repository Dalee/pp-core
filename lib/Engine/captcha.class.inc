<?php
class PXEngineCaptcha extends PXEngine {
	var $result;

	var $db   = array('factory' => 'PXDatabaseNull');
	var $user = array('factory' => 'PXUserNull');

	function initDirectories() {
	}

	function initModules() {
	}

	function runModules() {
	}
	
	public function engineClass(){
		return PX_ENGINE_SBIN;
	}
}
?>