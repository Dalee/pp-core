<?php

use PP\Plugin\PluginAbstract;

class PXPluginZoom extends PluginAbstract {
	function initialize($app) {
		$this->load("lib/zoom_filter.class.inc");
		$this->loadTrigger("system/zoom");
	}
}
