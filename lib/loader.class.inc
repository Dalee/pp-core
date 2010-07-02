<?

function __autoload($className) {
	if(!true) {
		d2(PXLoader::getInstance()->load_map);
		d2($className);
	}

	$f = PXLoader::find($className);

	if(file_exists($f)) {
		require_once  $f;
	} 

	/*else {
		@unlinkDir(BASEPATH . "/tmp/cache/config");
		@unlink(BASEPATH . "/tmp/cache/loader");
	}*/
}

class PXLoader {
	private $path = "";
	private static $inst =  null;
	private $cached_map = array();

	public  $load_map = array();

	private function __construct($path) {
		$this->setPath($path);
	}

	private function global_check() {
		$not_exists = array();

		foreach($this->load_map as $k=>$v) {
			if(!(!file_exists($v) && $not_exists[] = array($k, $v))) {
				!(class_exists($k) or interface_exists($k)) && $not_exists[] = array($k, "class not exist");
			}
		}

		if(!empty($not_exists)) {
			FatalError(sprintf("<pre>%s</pre>", var_export($not_exists, true)));
		}
	}

	function __destruct() {
		if(file_exists(BASEPATH . "/site/var/development")) {
			$this->global_check();
		}

		$o = self::$inst;

		$diff = array_diff($o->load_map, $o->cached_map);

		if(!empty($diff)) {
			file_put_contents(BASEPATH . "/tmp/cache/loader", serialize($o));
		}
	}

	public static function getInstance($path=null) {
		if(!is_object(self::$inst) && file_exists($cache_path = BASEPATH . "/tmp/cache/loader")) {
			self::$inst = @unserialize(file_get_contents($cache_path));
			self::$inst->cached_map = self::$inst->load_map;
		}

		if(is_object(self::$inst)) {
			self::$inst->setPath($path);
			return self::$inst;
		}

		return self::$inst = new PXLoader($path);
	}

	
	public static function find($class) {
		return @self::getInstance()->load_map[strtolower($class)];
	}

	public function setPath($path) {
		$this->path = strlen($path) ? $path : $this->path;
		return $this;
	}

	public function load($class, $path=null) {
		$this->load_map[strtolower($class)] = 
			realpath(sprintf("%s/%s", $this->path, $path));

		return $this;
	}
}

?>
