<?php

use PP\Plugin\PluginAbstract;

class PXPluginFilters extends PluginAbstract {
	private static $enabled = false;

	function initialize($app) {
		$this->loadTrigger("database/filters");
		$this->loadTrigger("system/filters");

		$this->loadOnlyInAdmin("lib/widget.class.inc");
		$this->loadOnlyInAdmin("decorators.php");
	}

	public static function toggle($on_off) {
		self::$enabled = !!$on_off;
	}

	public static function enabled() {
		return self::$enabled;
	}
}
