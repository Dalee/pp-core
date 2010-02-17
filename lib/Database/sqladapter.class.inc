<?php
require_once 'Cache/classes.inc';

class NLSQLDatabaseAdapter {
	private   $dbDriver;
	protected $selfDescription; // ?

	function NLSQLDatabaseAdapter(&$dbDescription) {
		$this->init($dbDescription);
	}

	function init($dbDescription) {
		$this->dbDriver        = $dbDescription->getDriver();
		$this->selfDescription = $dbDescription;
	}

	function switchDatabase($dbDescription) {
		$this->close();
		$this->init($dbDescription);
	}

	function __get($property) {
		switch($property) {
			case 'db':
			case 'connection':
				FatalError('You cant use deprecated $'.$property.' property');
				break;

			case 'cache':
				return $this->dbDriver->cache;
				break;
		}
	}

	function __call($method, $args) {
		return call_user_func_array(array($this->dbDriver, $method), $args);
	}

	function query($query, $donotusecache = false, $limitpair=NULL) {
		return $this->dbDriver->query($query, $donotusecache, $limitpair);
	}

	function TrueStatusString($status='TRUE') {
		return ($status == 'TRUE' || $status == 1) ? "'1'" : "'0'";
	}

	function ClearCache($force=false) {
		return $this->dbDriver->cache->clear();
	}

	function setCache($on) {
		return $on ? $this->cacheOn() : $this->cacheOff();
	}

	function cacheOn(){
		$this->dbDriver->setCache($this->selfDescription->cache);
	}

	function cacheOff(){
		$this->dbDriver->setCache(false);
	}

	function LIKE($condition, $percs = NULL ){
		return $this->dbDriver->LIKE($condition, is_null($percs) ? P_LEFT|P_RIGHT : $percs);
	}
}
?>