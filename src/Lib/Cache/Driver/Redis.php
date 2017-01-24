<?php

namespace PP\Lib\Cache\Driver;

use PP\Lib\Cache\CacheInterface;

/**
 * Class PXCacheRedis
 * @package PP\Lib\Cache\Driver
 */
class Redis implements CacheInterface {
	/** @var \Redis */
	protected $impl = null;
	protected $host = "localhost";
	protected $port = 6379;
	protected $database = 0;
	protected $cacheDomain = null;

	public function __construct($cacheDomain = null, $defaultExpire = 3600, $connectorArgs = null) {
		extension_loaded("redis") or FatalError(get_class($this) . " error: redis extension doesn't loaded or installed");

		$this->impl = new \Redis();
		$this->host = empty($connectorArgs['host']) ? $this->host : $connectorArgs['host'];
		$this->port = empty($connectorArgs['port']) ? $this->port : intval($connectorArgs['port']);
		$this->database = empty($connectorArgs['path']) ? $this->database : intval(ltrim($connectorArgs['path'], '/'));
		$this->cacheDomain = $cacheDomain;
		$this->connect();
	}

	private function connect() {
		try {
			$this->impl->connect($this->host, $this->port, 1);
			$this->impl->setOption(\Redis::OPT_SERIALIZER, \Redis::SERIALIZER_PHP);
			if (!empty($this->cacheDomain)) {
				$this->impl->setOption(\Redis::OPT_PREFIX, $this->cacheDomain.':');
			}
			$this->impl->select($this->database);
		} catch (\Exception $e) {
			trigger_error("Can't connect to Redis: {$this->host}:{$this->port}");
			$this->impl = null;
		}
	}

	private function key($key, $glob = false) {
		if (is_array($key)) {
			$keyPart   = $this->key(array_shift($key));
			$groupPart = $this->key(array_shift($key));
			return $groupPart . '_' . $keyPart;
		}
		return md5($key) . ($glob ? '_*' : '');
	}

	public function exists($key) {
		if (!$this->impl) {
			return false;
		}
		return $this->impl->exists($this->key($key));
	}

	public function save($key, $data, $expTime = null) {
		if (!$this->impl) {
			return false;
		}
		return $this->impl->set($this->key($key), $data, $expTime);
	}

	public function load($key) {
		if (!$this->impl) {
			return null;
		}
		$data = $this->impl->get($this->key($key));
		$data = ($data === false) ? null : $data;
		return $data;
	}

	public function increment($key, $offset = 1 , $initial = 0, $expTime = null) {
		if (!$this->impl) {
			return $initial;
		}
		$key = $this->key($key);
		$this->impl->exists($key) || $this->impl->set($key, $initial);
		return $this->impl->incrBy($key, $offset);
	}

	public function delete($key) {
		if (!$this->impl) {
			return false;
		}
		return $this->impl->delete($this->key($key));
	}

	public function clear() {
		if (!$this->impl) {
			return false;
		}
		return $this->impl->flushDB();
	}

	public function deleteGroup($group) {
		if (!$this->impl) {
			return false;
		}
		$keys = $this->impl->keys($this->key($group, true));
		$ok = true;
		foreach ($keys as $key) {
			$ok = $this->impl->delete($key) && $ok;
		}
		return $ok;
	}
}
