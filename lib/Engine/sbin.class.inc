<?php

use PP\Lib\Engine\AbstractEngine;

class PXEngineSbin extends AbstractEngine {
	var $result;

	var $request = array('factory' => 'PXNullRequest');
	var $user    = array('factory' => 'PXUserCron');

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
