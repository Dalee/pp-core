<?php

class PXTriggerSystemFilters extends PXAbstractSystemTrigger {

	function onAfterEngineStart($engine) {
		if ($engine->engineClass() != PX_ENGINE_ADMIN) {
			return;
		}

		PXPluginFilters::toggle(true);
	}
}

?>
