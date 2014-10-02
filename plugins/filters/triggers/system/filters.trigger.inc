<?php

class PXTriggerSystemFilters extends PXAbstractSystemTrigger {

	function OnAfterEngineStart($engine) {
		if ($engine->engineClass() != PX_ENGINE_ADMIN) {
			return;
		}
		
		PXPluginFilters::toggle(true);
	}
}

?>
