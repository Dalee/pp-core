<?php

class PXPluginMultiRegions extends PXAbstractPlugin {
	function initialize($app) {
		$plugin_path = dirname($this->description->getPathToPlugin());
		require_once $plugin_path . '/lib/staticregion.class.inc';

		PXLoader::getInstance($plugin_path . "/lib")
			-> load("PXMultiregions",            "multipleregions.class.inc")
			-> load("PXTabbedMenuRegionChanger", "widget.class.inc")
			-> load("PXDisplayTypeMultiRegions", "display_type.class.inc");

		require_once ($plugin_path . "/triggers/database/multipleregions.trigger.inc");
		require_once ($plugin_path . "/triggers/system/multipleregions.trigger.inc");
		
		$app
			->registerTrigger("database", array("name" => "multipleregions"))
			->registerTrigger("system",   array("name" => "multipleregions"));

		if ($app->engine->engineClass() == PX_ENGINE_ADMIN) {
			require_once $plugin_path . '/lib/storage_type.class.inc';
			require_once $plugin_path . '/decorators.php';
		}
	}
}

?>
