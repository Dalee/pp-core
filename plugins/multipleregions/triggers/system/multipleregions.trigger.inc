<?php

class PXTriggerSystemMultipleregions extends PXAbstractSystemTrigger {
	private $engines = array(PX_ENGINE_USER => 'user', PX_ENGINE_ADMIN => 'admin');

	function OnAfterEngineStart($engine) {
		$plugin = PXMultiRegions::getInstance($this->engines[$engine->engineClass()]);
		$plugin->setRid();
	}
}

?>
