<?php

namespace PP\Lib\Cache\Driver;

use PP\Lib\Cache\CacheInterface;
use PP\Serializer\DefaultSerializer;
use PP\Serializer\SerializerAwareInterface;
use PP\Serializer\SerializerAwareTrait;
use \Redis as RedisDriver;

/**
 * Class PXCacheRedis
 * @package PP\Lib\Cache\Driver
 *
 * Usage: redis://127.0.0.1:6379/0?timeout=2.0
 *
 * Explanation:
 *  * Redis instance running on 127.0.0.1:6379
 *  * use Redis database number "0" (Redis accept database numbers from 0 to 15, by default)
 *  * connection timeout is set to 2.0 seconds (float)
 *
 */
class Redis implements CacheInterface, SerializerAwareInterface {
	use SerializerAwareTrait;

	/** @var \Redis */
	protected $connection;

	/** @var string */
	protected $cachePrefix = '';

	/** @var string */
	protected $host;

	/** @var int */
	protected $port;

	/** @var float connection timeout, default is 1.5 */
	protected $timeout;

	/** @var int database number */
	protected $database = 0;

	/**
	 * Redis cache driver constructor.
	 *
	 * @param null|string $cacheDomain
	 * @param int $defaultExpire
	 * @param null|array $connectorArgs
	 */
	public function __construct($cacheDomain = null, $defaultExpire = 3600, $connectorArgs = null) {
		if (!extension_loaded('redis')) {
			FatalError('Redis extension is not loaded!');
		}

		// parse additional arguments..
		$paramsRaw = getFromArray($connectorArgs, 'query', '');
		parse_str($paramsRaw, $params);

		// create connection..
		$this->host = getFromArray($connectorArgs, 'host', '127.0.0.1');
		$this->port = getFromArray($connectorArgs, 'port', 6379);
		$this->database = empty($connectorArgs['path']) ? $this->database : intval(ltrim($connectorArgs['path'], '/'));
		$this->cachePrefix = ($cacheDomain === null) ? '' : $cacheDomain . ':';
		$this->timeout = (float)getFromArray($params, 'timeout', 1.5);
		$this->serializer = new DefaultSerializer();
		$this->connect();
	}

	/**
	 * Initiate Redis non-persistent connection.
	 */
	private function connect() {
		$this->connection = new RedisDriver();
		$this->connection->connect(
			$this->host,
			$this->port,
			$this->timeout
		);

		if (!empty($this->cachePrefix)) {
			$this->connection->setOption(
				RedisDriver::OPT_PREFIX,
				$this->cachePrefix
			);
		}
		$this->connection->select($this->database);
	}

	/**
	 * Convert Proxima key into Redis key.
	 *
	 * @param string $key
	 * @param bool $glob
	 * @return string
	 */
	private function key($key, $glob = false) {
		if (is_array($key)) {
			$keyPart = $this->key(array_shift($key));
			$groupPart = $this->key(array_shift($key));
			return $groupPart . '_' . $keyPart;
		}
		return md5($key) . ($glob ? '_*' : '');
	}

	/**
	 * {@inheritdoc}
	 */
	public function exists($key) {
		return $this->connection->exists($this->key($key));
	}

	/**
	 * {@inheritdoc}
	 */
	public function save($key, $data, $expTime = 3600) {
		$serialized = $this->serializer->serialize($data);
		return $this->connection->set($this->key($key), $serialized, $expTime);
	}

	/**
	 * {@inheritdoc}
	 */
	public function load($key) {
		$data = $this->connection->get($this->key($key));
		$unserialized = $this->serializer->unserialize($data);
		$unserialized = $unserialized === false ? null : $data;

		return $unserialized;
	}

	/**
	 * {@inheritdoc}
	 */
	public function increment($key, $offset = 1, $initial = 0, $expTime = null) {
		$key = $this->key($key);
		if (!$this->connection->exists($key)) {
			$this->connection->set($key, $initial);
		}

		return $this->connection->incrBy($key, $offset);
	}

	/**
	 * {@inheritdoc}
	 */
	public function delete($key) {
		return $this->connection->delete($this->key($key));
	}

	public function clear() {
		return $this->connection->flushDB();
	}

	/**
	 * {@inheritdoc}
	 *
	 * @see https://github.com/phpredis/phpredis/issues/1117
	 */
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
