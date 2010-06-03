<?php

class PXPluginMultiRegions extends PXAbstractPlugin {
	function initialize($app) {
		$plugin_path = dirname($this->description->getPathToPlugin());
		require_once $plugin_path . '/lib/multipleregions.class.inc';

		if ($app->engine->engineClass() == PX_ENGINE_ADMIN) {
			require_once $plugin_path . '/lib/widget.class.inc';
			require_once $plugin_path . '/lib/display_type.class.inc';
			require_once $plugin_path . '/decorators.php';
		}
	}
}

?>
