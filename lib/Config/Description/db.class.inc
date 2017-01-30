<?php

use PP\Properties\EnvLoader;
use PP\Lib\Database\DatabaseException;
use PP\Lib\Database\Driver\PostgreSqlDriver;

define('ERROR_DB_CANNOTCONNECT', -1);
define('ERROR_DB_BADQUERY', -2);

class NLDBDescription {
	//REMOVE THESE STUPID FIELD DESCRIPTIONS !!!
	private $user = '';
	private $password = '';

	private $dbtype; // FIXME
	private $dbname = '';
	private $dbids;

	private $host = '';
	private $port = 0;

	private $options;
	private $tty;

	public $cache;
	public $aicache; // private or public? I don't know
	private $encoding;

	public function __construct($config) {
		foreach ($config as $k => $v) {
			$this->$k = $v;
		}
	}

	/**
	 *
	 * @return static
	 */
	public static function fromEnv() {
		$config = EnvLoader::getMappedArray([
			'DB_USER' => 'user',
			'DB_PASSWORD' => 'password',
			'DB_TYPE' => 'dbtype',
			'DB_NAME' => 'dbname',
			'DB_HOST' => 'host',
			'DB_PORT' => 'port',
			'DB_CACHE' => 'cache',
			'DB_ENCODING' => 'encoding',
		]);

		return new static($config);
	}

	//TODO: remove magic constants
	function __get($name) {
		$val = null;
		if (property_exists($this, $name)) {
			$val = $this->$name;
			if (empty($val)) {
				$val = null;
			}
		}

		return $val;
	}

	function __set($name, $value) {
		$this->$name = $value;
	}
	//

	/**
	 *
	 * @return PostgreSqlDriver
	 * @throws \PP\Lib\Database\DatabaseException
	 */
	public function getDriver() {
		$db = null;

		switch ($this->dbtype) {
			case PostgreSqlDriver::TYPE:
				$db = new PostgreSqlDriver($this);
				break;

			default:
				throw new DatabaseException("Can't find class: {$this->dbtype}");
				break;
		}

		return $db;
	}
}
