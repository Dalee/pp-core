<?php

use PP\Plugin\AbstractPlugin;

class PXPluginZoom extends AbstractPlugin {
	function initialize($app) {
		$this->load("lib/zoom_filter.class.inc");
		$this->loadTrigger("system/zoom");
	}
}
