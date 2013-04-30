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

		array_map(array($this, "loadModule"),  $this->description->modules);
		array_map(array($this, "loadTrigger"), $this->description->triggers);

		$this->initialize($app);
	}

	function initialize($app) {
	}

	function initSet($params = null) {
	}

	public static function getParam($pluginName, $paramName) {
		return @PXRegistry::getApp()->plugins[$pluginName]->params[$paramName];
	}

	function loadTrigger($relativePath) {
		list($type, $name) = explode("/", $relativePath);
		$className = sprintf("pxtrigger%s%s", $type, $name);
		$this->app->registerTrigger($type, array("name" => $name) + array("folder" => $this->name));
	}

	function load($path, $pattern="%s") {
		require_once sprintf("%s/{$pattern}", $this->path, $path);
	}

	function loadModule($relativePath) {
		$this->load($relativePath, "modules/%s.module.inc");
	}

	function loadCronrun($relativePath) {
		$this->load($relativePath, "cronruns/%s.cronrun.inc");
	}

	function loadDisplayType($filename_without_ext) {
		$this->loadWithLoader('displayTypes', 'PXDisplayType', $filename_without_ext);
	}

	function loadStorageType($filename_without_ext) {
		$this->loadWithLoader('storageTypes', 'PXStorageType', $filename_without_ext);
	}

	function loadWithLoader($folder, $classPrefix, $filename_without_ext, $extension='class.inc') {
		PXLoader::getInstance("{$this->path}/{$folder}/")
			->load("{$classPrefix}{$filename_without_ext}", "{$filename_without_ext}.{$extension}");
	}

	function loadOnlyInAdmin($path) {
		if ($this->app->isAdminEngine()) {
			$this->load($path);
		}
	}

	public static function autoload($className) {
		if (!true) {
			d2(PXLoader::getInstance()->load_map);
			d2($className);
		}

		$f = PXLoader::find($className);

		if (!strstr($f, "/plugins/")) {
			return;
		}

		if (file_exists($f)) {
			require_once  $f;
		} else {
			@unlinkDir(CACHE_PATH . "/config");
			@unlink(CACHE_PATH . "/loader");
		}
	}
}

?>
