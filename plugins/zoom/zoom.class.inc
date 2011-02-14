<?php

class PXPluginZoom extends PXAbstractPlugin {
	function initialize($app) {
		$this->load("lib/zoom_filter.class.inc");
		$this->loadTrigger("system/zoom");
	}
}
?>
