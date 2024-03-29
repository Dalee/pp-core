<?php

use PP\Properties\EnvLoader;
use PP\Lib\Database\DatabaseException;
use PP\Lib\Database\Driver\PostgreSqlDriver;

define('ERROR_DB_CANNOTCONNECT', -1);
define('ERROR_DB_BADQUERY', -2);

class NLDBDescription
{

	private $user = '';
	private $password = '';
	private $dbtype;
	private $dbname = '';
	private $host = '';
	private $port = 0;
	private $options;
	private $encoding;

	public $cache;

	public function __construct($config)
	{
		foreach ($config as $k => $v) {
			$this->$k = $v;
		}
	}

	/**
	 * Create DbDescription from environment variables.
	 *
	 * @return static
	 */
	public static function fromEnv()
	{
		$q = [];
		$r = parse_url(EnvLoader::get('DATABASE_DSN'));
		parse_str((string) getFromArray($r, 'query', ''), $q);

		// either cache is supplied via &cache parameter of DATABASE_DSN
		// or cache is defined as additional environment variable.
		$cache = getFromArray($q, 'cache');
		$cacheOverride = EnvLoader::get('DATABASE_CACHE');
		if ($cacheOverride) {
			$cache = $cacheOverride;
		}

		$config = [
			'dbtype' => getFromArray($r, 'scheme', ''),
			'user' => getFromArray($r, 'user', ''),
			'password' => getFromArray($r, 'pass', ''),
			'dbname' => ltrim((string) getFromArray($r, 'path', ''), '/'),
			'host' => getFromArray($r, 'host', ''),
			'port' => getFromArray($r, 'port', ''),
			'encoding' => getFromArray($q, 'encoding', DEFAULT_CHARSET),
			'cache' => $cache,
		];

		return new static($config);
	}

	//TODO: remove magic constants
	public function __get($name)
	{
		$val = null;
		if (property_exists($this, $name)) {
			$val = $this->$name;
			if (empty($val)) {
				$val = null;
			}
		}

		return $val;
	}

	public function __set($name, $value)
	{
		$this->$name = $value;
	}

	/**
	 *
	 * @return PostgreSqlDriver
	 * @throws DatabaseException
	 */
	public function getDriver()
	{
		$db = null;

		$db = match ($this->dbtype) {
			PostgreSqlDriver::TYPE => new PostgreSqlDriver($this),
			default => throw new DatabaseException("Can't find class: {$this->dbtype}"),
		};

		$db->setCache($this->cache);
		return $db;
	}
}
