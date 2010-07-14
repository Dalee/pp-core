<?php

class PXPluginMultiRegions extends PXAbstractPlugin {
	function initialize($app) {
		require_once $this->path . '/lib/staticregion.class.inc';

		PXLoader::getInstance($this->path . "/lib")
			-> load("PXMultiregions",            "multipleregions.class.inc")
			-> load("PXTabbedMenuRegionChanger", "widget.class.inc")
			-> load("PXDisplayTypeMultiRegions", "display_type.class.inc");

		$this->loadTrigger("PXTriggerDatabaseMultipleregions");
		$this->loadTrigger("PXTriggerSystemMultipleregions");

		if ($app->isAdminEngine()) {
			require_once $this->path . '/lib/storage_type.class.inc';
			require_once $this->path . '/decorators.php';
		}
	}
}

?>