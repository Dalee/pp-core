<?php

namespace PP\Lib\Database;

/**
 * Class DatabaseAdapter
 * @package PP\Lib\Database
 *
 * @method void close()
 */
class DatabaseAdapter {
	protected $dbDriver;
	protected $selfDescription; // ?

	public function __construct(&$dbDescription) {
		$this->init($dbDescription);
	}

	/**
	 * @param \NLDBDescription $dbDescription
	 */
	function init($dbDescription) {
		$this->dbDriver = $dbDescription->getDriver();
		$this->selfDescription = $dbDescription;
	}

	function switchDatabase($dbDescription) {
		$this->close();
		$this->init($dbDescription);
	}

	function __get($property) {
		switch ($property) {
			case 'db':
			case 'connection':
				FatalError('You cant use deprecated $' . $property . ' property');
				break;

			case 'cache':
				return $this->dbDriver->cache;
				break;
		}
	}

	function __call($method, $args) {
		if (!method_exists($this->dbDriver, $method)) {
			FatalError('Undefined method ' . $method . ' in ' . get_class($this->dbDriver));
		}

		return call_user_func_array([$this->dbDriver, $method], $args);
	}

	function query($query, $donotusecache = false, $limitpair = null) {
		return $this->dbDriver->query($query, $donotusecache, $limitpair);
	}

	function TrueStatusString($status = 'TRUE') {
		return ($status == 'TRUE' || $status == 1) ? "'1'" : "'0'";
	}

	function ClearCache($force = false) {
		return $this->dbDriver->cache->clear();
	}

	function setCache($on) {
		return $on ? $this->cacheOn() : $this->cacheOff();
	}

	function cacheOn() {
		$this->dbDriver->setCache($this->selfDescription->cache);
	}

	function cacheOff() {
		$this->dbDriver->setCache(false);
	}

	function getSelfDescription() {
		return $this->selfDescription;
	}

	function LIKE($condition, $percs = null) {
		return $this->dbDriver->LIKE($condition, is_null($percs) ? P_LEFT | P_RIGHT : $percs);
	}
}
