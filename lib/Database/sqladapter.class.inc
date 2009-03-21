<?php
require_once 'Cache/classes.inc';

class NLSQLDatabaseAdapter {
	private   $dbDriver;
	protected $cache;

	function NLSQLDatabaseAdapter(&$dbDescription) {
		$this->init($dbDescription);
	}

	function init($dbDescription) {
		$this->dbDriver = $dbDescription->getDriver();
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
		return $this->db->cache->clear();
	}
	
	function LIKE($condition, $percs = NULL ){
		return $this->db->LIKE($condition, is_null($percs) ? P_LEFT|P_RIGHT : $percs);
	}
}
?>
