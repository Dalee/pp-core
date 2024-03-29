<?php

class PXLoader
{
	private $path = "";
	private static $inst = null;
	private array $cached_map = [];

	public $load_map = [];

	private function __construct($path)
	{
		$this->setPath($path);
	}

	public function __destruct()
	{
		$o = self::$inst;

		$diff = array_diff($o->load_map, $o->cached_map);

		if (!empty($diff)) {
			file_put_contents(CACHE_PATH . "/loader", serialize($o));
		}
	}

	public static function getInstance($path = null)
	{
		if (!is_object(self::$inst) && file_exists($cache_path = CACHE_PATH . "/loader")) {
			self::$inst = @unserialize(file_get_contents($cache_path));
			self::$inst->cached_map = self::$inst->load_map;
		}

		if (is_object(self::$inst)) {
			self::$inst->setPath($path);
			return self::$inst;
		}

		return self::$inst = new PXLoader($path);
	}


	public static function find($class)
	{
		return @self::getInstance()->load_map[mb_strtolower((string) $class)];
	}

	public function setPath($path)
	{
		$this->path = mb_strlen((string) $path) ? $path : $this->path;
		return $this;
	}

	public function load($class, $path = null)
	{
		$this->load_map[mb_strtolower((string) $class)] =
			realpath(sprintf("%s/%s", $this->path, $path));

		return $this;
	}
}
