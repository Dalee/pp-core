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

	function loadWithLoader($folder, $classPrefix, $filename_without_ext, $extension='class.inc') {
		PXLoader::getInstance("{$this->path}/{$folder}/")
			->load("{$classPrefix}{$filename_without_ext}", "{$filename_without_ext}.{$extension}");
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

	function loadOnlyInAdmin($path) {
		if ($this->app->isAdminEngine()) {
			$this->load($path);
		}
	}


	/**
	 * allow dynamically add methods for plugins
	 * but waiting for php5.4 to bind $this for $this->path and $this->load()
	private static $dynamicMethods = array();
	protected static function declareMethod ($name, $callback) {
		if (isset(self::$dynamicMethods[$name])) {
			throw new Exception('Cannot redeclare method.');
		}
		self::$dynamicMethods[$name] = $callback;
	}
	public function __call($name, $args) {
		if (!isset(self::$dynamicMethods[$name])) {
			throw new Exception('Call to undefined method '.__CLASS__.'::'.$name.'()');
		}
		$callback = self::$dynamicMethods[$name];
		if (!is_callable($callback)) {
			throw new Exception('Invalid callback declared for '.__CLASS__.'::'.$name.'()');
		}
		// waiting for php5.4 to bind $this for $this->path and $this->load()
		// d20(compact('name', 'args'));debug_print_backtrace();die;
		return call_user_func_array($callback, $args);
	}
	*/

	// what the hell is that?
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
