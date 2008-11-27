<?php
define('ERROR_DB_CANNOTCONNECT', -1);
define('ERROR_DB_BADQUERY',      -2);

class NLDBDescription {
	private $host;
	private $port;

	private $user;
	private $password;

	private $dbname;
	public $dbtype; // FIXME
	private $dbids;

	private $options;
	private $tty;

	public  $cache;
	public  $encoding;

	function __construct($config) {
		if (isset($config['host']))     $this->host     = $config['host'];
		if (isset($config['port']))     $this->port     = $config['port'];
		if (isset($config['dbname']))   $this->dbname   = $config['dbname'];
		if (isset($config['dbtype']))   $this->dbtype   = $config['dbtype'];
		if (isset($config['dbids']))    $this->dbids    = $config['dbids'];
		if (isset($config['user']))     $this->user     = $config['user'];
		if (isset($config['password'])) $this->password = $config['password'];
		if (isset($config['options']))  $this->options  = $config['options'];
		if (isset($config['tty']))      $this->tty      = $config['tty'];
		if (isset($config['cache']))    $this->cache    = $config['cache'];
		if (isset($config['encoding'])) $this->encoding = $config['encoding'];
	}

	function getDriver() {
		switch ($this->dbtype) {
			case 'pgsql':
				require_once('Database/SQL/pgsql.class.inc');
				return new NLPGSQLDatabase($this);
				break;

			case 'mssql':
				require_once('Database/SQL/mssql.class.inc');
				return new NLMSSQLDatabase($this);
				break;

			case 'mysql':
				require_once('Database/SQL/mysql.class.inc');
				return new NLMYSQLDatabase($this);
				break;

			case NULL:
			case '':
				FatalError("Вы не выбрали тип базы данных");
				break;
			default:
				FatalError("Указан некорректный тип базы данных");
				break;
		}
	}

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