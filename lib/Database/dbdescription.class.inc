<?php
define('ERROR_DB_CANNOTCONNECT', -1);
define('ERROR_DB_BADQUERY',      -2);

class NLDBDescription {

	var $host;
	var $port;
	var $user;
	var $password;
	var $dbname;
	var $dbtype;
	var $dbids;
	var $options;
	var $tty;
	var $cache;
	var $encoding;

	function GetPgsqlConnectString() {
		$connectString = '';
		if ($this->host)     $connectString .= ' host =   '.$this->host;
		if ($this->port)     $connectString .= ' port =   '.$this->port;
		if ($this->user)     $connectString .= ' user =   '.$this->user;
		if ($this->password) $connectString .= ' passwd = '.$this->password;
		if ($this->dbname)   $connectString .= ' dbname = '.$this->dbname;
		return trim($connectString);
	}

	function GetMssqlConnectArray() {
		$connectArray = array();
		$connectArray['host']     = $this->host     ? $this->host   : 'LOCALHOST';
		$connectArray['port']     = $this->port     ? $this->port   : '0';
		$connectArray['user']     = $this->user     ? $this->user   : 'admin';
		$connectArray['password'] = $this->password ? $this->password : '';
		$connectArray['dbname']   = $this->dbname   ? $this->dbname : 'test';
		return $connectArray;
	}

	function GetMysqlConnectArray() {
		$connectArray = array();
		$connectArray['host']   = $this->host     ? $this->host   : 'localhost';
		$connectArray['port']   = $this->port     ? $this->port   : '3306';
		$connectArray['user']   = $this->user     ? $this->user   : 'admin';
		$connectArray['passwd'] = $this->password ? $this->password : '';
		$connectArray['dbname'] = $this->dbname   ? $this->dbname : 'test';
		return $connectArray;
	}
}

?>
