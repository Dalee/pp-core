<?php
class PXPluginMultiRegions extends PXAbstractPlugin {
	function initialize($app) {
		$this->load("lib/staticregion.class.inc");
		$this->load("lib/cloner.class.inc");
		$this->load("lib/helper.class.inc");
		$this->load("lib/multipleregions.class.inc");
		$this->load("lib/reflexer.class.inc");

		$this->loadDisplayType("multiregions");
		$this->loadDisplayType("multiregions_static");
		$this->loadDisplayType("urltable");
		$this->loadStorageType("regionurl");

		$this->loadTrigger("database/multipleregions");
		$this->loadTrigger("system/multipleregions");

		$this->loadOnlyInAdmin("lib/widget.class.inc");
		$this->loadOnlyInAdmin("decorators.php");
	}
}
?>