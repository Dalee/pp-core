<?php

class PXAbstractPlugin {
	protected $name = null;
	public    $description;

	public $app;

	function __construct($app, $description) {
		$this->app = $app;

		$this->description = $description;
		$this->name = $description->getName();
		$this->path = dirname($this->description->getPathToPlugin());
		$this->initialize($app);
	}

	function initialize($app) {
	}

	function initSet($params = null) {
	}

	function loadTrigger($className) {
		$path = strtr(strtolower($className), array(
			"pxtrigger"    => "triggers/", 
			"database"     => "database/",
			"storagetype"  => "storagetype/",
			"system"       => "system/"));

		require_once $this->path . "/" . $path . ".trigger.inc";

		$tmp = explode("/", $path);
		$this->app->registerTrigger($tmp[1], array("name" => end($tmp)));
	}

	public static function autoload($className) {
		if(!true) {
			d2(PXLoader::getInstance()->load_map);
			d2($className);
		}

		$f = PXLoader::find($className);

		if(!strstr($f, "/plugins/")) {
			return;
		}

		if(file_exists($f)) {
			require_once  $f;
		} else {
			@unlinkDir(BASEPATH . "/tmp/cache/config");
			@unlink(BASEPATH . "/tmp/cache/loader");
		}
	}
}

?>
