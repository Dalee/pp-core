<?php
define('ERROR_DB_CANNOTCONNECT', -1);
define('ERROR_DB_BADQUERY',      -2);

class NLDBDescription {
	private $user;
	private $password;

	private $dbname;
	public  $dbtype; // FIXME
	private $dbids;

	private $host;
	private $port;

	private $options;
	private $tty;

	public  $cache;
	public  $aicache; // private or public? I don't know
	public  $encoding;

	function __construct($config) {
		foreach(get_object_vars($this) as $k=>$v) {
			if(isset($config[$k])) {
				$this->$k = $config[$k];
			}
		}
	}

	//RESOLVE ME!
	function __get($name) {
		return $this->$name;
	}

	function __set($name, $value) {
		$this->$name = $value;
	}
	//

	function getDriver() {
		$class_file = "Database/SQL/{$this->dbtype}.class.inc";

		if(file_exists(BASEPATH . "/libpp/lib/" . $class_file)) {
			require_once($class_file);
			$class_name = "NL".strtoupper($this->dbtype)."Database";

			$db = new $class_name($this);
			return $db;
		} else {
			FatalError("Вы не выбрали тип базы данных");
			FatalError("Указан некорректный тип базы данных");
		}
	}

	function GetPgsqlConnectString() {
		$connection = array();

		foreach(array("host", "port", "user", "password", "dbname") as $field) {
			if ($this->$field) {
				$connection[] =  sprintf("%s=%s", $field, $this->$field);
			}
		}

		 return trim(join(" ", $connection));
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
