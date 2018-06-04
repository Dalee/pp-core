<?php

namespace PP\Lib\Cache\Driver;

use PP\Lib\Cache\CacheInterface;
use PP\Serializer\SerializerAwareInterface;
use PP\Serializer\SerializerAwareTrait;
use PP\Serializer\DefaultSerializer;
use Predis\Client;
use Predis\Collection\Iterator\Keyspace;

/**
 * Class Predis
 *
 * Usage: predis://127.0.0.1:6379/0?timeout=2.0
 *
 * @package PP\Lib\Cache\Driver
 */
class Predis implements CacheInterface, SerializerAwareInterface {
	use SerializerAwareTrait;

	/**
	 * @var Client
	 */
	protected $client;

	/**
	 * @var string
	 */
	protected $cachePrefix = '';

	/**
	 * @var int
	 * @see https://redis.io/commands/scan#the-count-option
	 */
	protected $scanDefault = 50;

	/**
	 * @return int
	 */
	public function getScanDefault() {
		return $this->scanDefault;
	}

	/**
	 * @param int $scanDefault
	 * @return $this
	 */
	public function setScanDefault($scanDefault) {
		$this->scanDefault = $scanDefault;
		return $this;
	}

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
		$this->serializer = new DefaultSerializer();
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
		$serialized = $this->serializer->serialize($data);
		$result = $this->client->set($this->key($key), $serialized, 'ex', $expTime);

		return $result->getPayload() === 'OK';
	}

	/**
	 * {@inheritdoc}
	 */
	public function load($key) {
		$data = $this->client->get($this->key($key));
		$unserialized = $this->serializer->unserialize($data);
		$unserialized = $unserialized === false ? null : $unserialized;

		return $unserialized;
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
		$pattern = $this->cachePrefix . $keyGroup;
		$keys = new Keyspace($this->client, $pattern, $this->scanDefault);
		foreach ($keys as $key) {
			$this->client->del([$key]);
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
