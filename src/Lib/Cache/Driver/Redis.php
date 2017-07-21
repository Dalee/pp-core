<?php

namespace PP\Lib\Cache\Driver;

use PP\Lib\Cache\CacheInterface;

/**
 * Class PXCacheRedis
 * @package PP\Lib\Cache\Driver
 *
 * BEWARE, please read this: https://github.com/phpredis/phpredis/issues/1117
 */
class Redis implements CacheInterface {

	/** @var \Redis */
	protected $connection;

	protected $cachePrefix = '';
	protected $host;
	protected $port;
	protected $database = 0;

	public function __construct($cacheDomain = null, $defaultExpire = 3600, $connectorArgs = null) {
		extension_loaded("redis") or FatalError(get_class($this) . " error: redis extension doesn't loaded or installed");

		$this->connection = new \Redis();
		$this->host = getFromArray($connectorArgs, 'host', '127.0.0.1');
		$this->port = getFromArray($connectorArgs, 'port', 6379);
		$this->database = empty($connectorArgs['path']) ? $this->database : intval(ltrim($connectorArgs['path'], '/'));
		$this->cachePrefix = ($cacheDomain === null) ? '' : $cacheDomain . ':';
		$this->connect();
	}

	private function connect() {
		$this->connection->connect($this->host, $this->port, 1);
		$this->connection->setOption(\Redis::OPT_SERIALIZER, \Redis::SERIALIZER_PHP);
		if (!empty($this->cachePrefix)) {
			$this->connection->setOption(\Redis::OPT_PREFIX, $this->cachePrefix);
		}
		$this->connection->select($this->database);
	}

	private function key($key, $glob = false) {
		if (is_array($key)) {
			$keyPart = $this->key(array_shift($key));
			$groupPart = $this->key(array_shift($key));
			return $groupPart . '_' . $keyPart;
		}
		return md5($key) . ($glob ? '_*' : '');
	}

	public function exists($key) {
		return $this->connection->exists($this->key($key));
	}

	public function save($key, $data, $expTime = 3600) {
		return $this->connection->set($this->key($key), $data, $expTime);
	}

	public function load($key) {
		$data = $this->connection->get($this->key($key));
		$data = ($data === false) ? null : $data;

		return $data;
	}

	public function increment($key, $offset = 1, $initial = 0, $expTime = null) {
		$key = $this->key($key);
		if (!$this->connection->exists($key)) {
			$this->connection->set($key, $initial);
		}

		return $this->connection->incrBy($key, $offset);
	}

	public function delete($key) {
		return $this->connection->delete($this->key($key));
	}

	public function clear() {
		return $this->connection->flushDB();
	}

	// @see https://github.com/phpredis/phpredis/issues/1117
	public function deleteGroup($group) {
		// find keys to delete
		$keyGroup = $this->key($group, true);
		$keys = $this->connection->keys($keyGroup);
		$cachePrefixLen = strlen($this->cachePrefix);

		foreach ($keys as $key) {
			if (substr($key, 0, $cachePrefixLen) === $this->cachePrefix) {
				$key = substr($key, $cachePrefixLen);
				$this->connection->del($key);
			}
		}

		return true;
	}
}
