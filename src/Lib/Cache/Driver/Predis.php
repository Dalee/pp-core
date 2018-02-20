<?php

namespace PP\Lib\Cache\Driver;

use PP\Lib\Cache\CacheInterface;
use Predis\Client;

/**
 * Class Predis
 *
 * Usage: predis://127.0.0.1:6379/0?timeout=2.0
 *
 * @package PP\Lib\Cache\Driver
 */
class Predis implements CacheInterface {

	/**
	 * @var Client
	 */
	protected $client;

	/**
	 * @var string
	 */
	protected $cachePrefix = '';

	/**
	 * Predis constructor.
	 *
	 * @param null $cacheDomain
	 * @param int $defaultExpire
	 * @param null $connectorArgs
	 */
	public function __construct($cacheDomain = null, $defaultExpire = 3600, $connectorArgs = null) {
		$this->cachePrefix = $cacheDomain === null ? '' : $cacheDomain . ':';
		$connectorArgs = str_replace('predis', 'redis', $connectorArgs);
		$this->client = new Client($connectorArgs, [
			'prefix' => $this->cachePrefix
		]);
	}

	/**
	 * {@inheritdoc}
	 */
	public function exists($key) {
		return $this->client->exists($this->key($key)) > 0;
	}

	/**
	 * {@inheritdoc}
	 */
	public function save($key, $data, $expTime = 3600) {
		$result = $this->client->set($this->key($key), serialize($data), 'ex', $expTime);
		return $result->getPayload() === 'OK';
	}

	/**
	 * {@inheritdoc}
	 */
	public function load($key) {
		$data = @unserialize($this->client->get($this->key($key)));
		$data = $data === false ? null : $data;

		return $data;
	}

	/**
	 * {@inheritdoc}
	 */
	public function increment($key, $offset = 1, $initial = 0, $expTime = null) {
		$key = $this->key($key);

		if ($this->client->exists($key) === 0) {
			$this->client->set($key, $initial);
		}

		return $this->client->incrby($key, $offset);
	}

	/**
	 * {@inheritdoc}
	 */
	public function delete($key) {
		return $this->client->del([$this->key($key)]) > 0;
	}

	/**
	 * {@inheritdoc}
	 */
	public function clear() {
		return $this->client->flushdb()->getPayload() === 'OK';
	}

	/**
	 * {@inheritdoc}
	 */
	public function deleteGroup($group) {
		$keyGroup = $this->key($group, true);
		$keys = $this->client->keys($keyGroup);
		$cachePrefixLen = strlen($this->cachePrefix);

		foreach ($keys as $key) {
			if (substr($key, 0, $cachePrefixLen) === $this->cachePrefix) {
				$key = substr($key, $cachePrefixLen);
				$this->client->del([$key]);
			}
		}

		return true;
	}

	/**
	 * Convert Proxima key into Redis key.
	 *
	 * @param $key
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

}
