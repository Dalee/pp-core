<?php
define('ERROR_DB_CANNOTCONNECT', -1);
define('ERROR_DB_BADQUERY',      -2);

class NLDBDescription {
	//REMOVE THESE STUPID FIELD DESCRIPTIONS !!!
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
		foreach($config as $k=>$v) {
			$this->$k = $v;
		}
	}

	//RESOLVE ME!
	function __get($name) {
		return (property_exists($this, $name) ? $this->$name : NULL);
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
}
?>
