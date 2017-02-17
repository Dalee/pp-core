<?php

use PP\Plugin\AbstractPlugin;

class PXPluginMultiRegions extends AbstractPlugin {

	function initialize($app) {
		$this->load("lib/staticregion.class.inc");
		$this->load("lib/cloner.class.inc");
		$this->load("lib/helper.class.inc");
		$this->load("lib/multipleregions.class.inc");
		$this->load("lib/reflexer.class.inc");

		$this->load("lib/events_callback.class.inc");
		$this->loadTrigger("database/events");

		$this->loadDisplayType("multiregions");
		$this->loadDisplayType("multiregions_static");
		$this->loadDisplayType("urltable");
		$this->loadStorageType("regionurl");
		$this->loadStorageType("federalflag");

		$this->loadTrigger("database/multipleregions");
		$this->loadTrigger("system/multipleregions");
		$this->loadTrigger("search/multipleregions");
		$this->loadTrigger("layout/multicreatepathmodify");

		$this->loadOnlyInAdmin("lib/widget.class.inc");
		$this->loadOnlyInAdmin("decorators.php");
	}
}

