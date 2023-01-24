<?php

namespace PP\Lib\Database;

/**
 * Class DatabaseAdapter
 * @package PP\Lib\Database
 *
 * @method void close()
 */
class DatabaseAdapter
{
	protected $dbDriver;
	protected $selfDescription; // ?

	public function __construct(&$dbDescription)
	{
		$this->init($dbDescription);
	}

	/**
	 * @param \NLDBDescription $dbDescription
	 */
	public function init($dbDescription)
	{
		$this->dbDriver = $dbDescription->getDriver();
		$this->selfDescription = $dbDescription;
	}

	public function switchDatabase($dbDescription)
	{
		$this->close();
		$this->init($dbDescription);
	}

	public function __get($property)
	{
		switch ($property) {
			case 'db':
			case 'connection':
				FatalError('You cant use deprecated $' . $property . ' property');
				break;

			case 'cache':
				return $this->dbDriver->cache;
		}
	}

	public function __call($method, $args)
	{
		if (!method_exists($this->dbDriver, $method)) {
			FatalError('Undefined method ' . $method . ' in ' . $this->dbDriver::class);
		}

		return call_user_func_array([$this->dbDriver, $method], $args);
	}

	public function query($query, $donotusecache = false, $limitpair = null)
	{
		return $this->dbDriver->query($query, $donotusecache, $limitpair);
	}

	public function TrueStatusString($status = 'TRUE')
	{
		return ($status == 'TRUE' || $status == 1) ? "'1'" : "'0'";
	}

	public function ClearCache($force = false)
	{
		return $this->dbDriver->cache->clear();
	}

	public function setCache($on)
	{
		return $on ? $this->cacheOn() : $this->cacheOff();
	}

	public function cacheOn()
	{
		$this->dbDriver->setCache($this->selfDescription->cache);
	}

	public function cacheOff()
	{
		$this->dbDriver->setCache(false);
	}

	public function getSelfDescription()
	{
		return $this->selfDescription;
	}

	public function LIKE($condition, $percs = null)
	{
		return $this->dbDriver->LIKE($condition, is_null($percs) ? P_LEFT | P_RIGHT : $percs);
	}
}
