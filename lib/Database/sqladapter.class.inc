<?php
require_once 'Config/Description/db.class.inc';

class NLSQLDatabaseAdapter {
	private   $dbDriver;
	protected $cache;

	function NLSQLDatabaseAdapter($dbDescription) {
		$this->init($dbDescription);
	}

	function init($dbDescription) {
		$this->dbDriver = $dbDescription->getDriver();
		$this->cache    = $dbDescription->cache;
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
}

?>
