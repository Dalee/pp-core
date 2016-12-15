<?php

use PP\Lib\Engine\AbstractEngine;

class PXTriggerSystemMultipleregions extends PXAbstractSystemTrigger {
	private $engines = array(PX_ENGINE_USER => 'user', PX_ENGINE_ADMIN => 'admin', PX_ENGINE_SBIN => 'script');

	/**
	 * @param AbstractEngine $engine
	 */
	function OnAfterEngineStart($engine) {
		$plugin = PXMultiRegions::getInstance($this->engines[$engine->engineClass()]);
		$plugin->setSession(PXRegistry::getSession()); // prevent opening session in plugin
		$plugin->setRid();
	}
}
