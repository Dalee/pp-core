<?php

namespace PP\Lib\Cache\Driver;

use PP\Lib\Cache\CacheInterface;

/**
 * Class Memcached
 * @package PP\Lib\Cache\Driver
 */
class Memcached implements CacheInterface
{
    /** @var \Memcached */
    private $connection;

    private readonly int $defaultExpireTime;

    private readonly string $cacheNamespace;

    /** @var string */
    public $host = 'localhost';

    /** @var int */
    public $port = 11211;

    public function __construct($cacheDomain = null, $defaultExpire = 3600, $connectorArgs = null)
    {
        if (!extension_loaded("memcached")) {
            throw new \Exception("Memcached extension is not loaded");
        }

        $this->defaultExpireTime = (int)$defaultExpire;
        $this->cacheNamespace = md5(BASEPATH . $cacheDomain);
        $this->host = getFromArray($connectorArgs, 'host', $this->host);
        $this->port = (int)getFromArray($connectorArgs, 'port', $this->port);
        $this->connection = $this->connect();
    }

    public function exists($key)
    {
        $this->connection->get($this->key($key));
        return $this->connection->getResultCode() !== \Memcached::RES_NOTFOUND;
    }

    public function save($key, $data, $expTime = null)
    {
        $expTime = (int)$expTime;
        $expTime = ($expTime > 0)
            ? $expTime
            : $this->defaultExpireTime;

        $this->connection->set($this->key($key), $data, $expTime);
    }

    public function load($key)
    {
        // look at that: https://github.com/php-memcached-dev/php-memcached/issues/21
        $res = $this->connection->get($this->key($key));
        return $this->connection->getResultCode() == \Memcached::RES_NOTFOUND ? null : $res;
    }

    public function delete($key)
    {
        $this->connection->delete($this->key($key));
    }

    public function deleteGroup($group)
    {
        $prefix = $this->key($group, true);
        $prefLen = mb_strlen((string) $prefix);
        $allKeys = $this->connection->getAllKeys();
        if (empty($allKeys)) {
            return;
        }

        $toDelete = [];
        foreach ($allKeys as $key) {
            if (mb_substr((string) $key, 0, $prefLen) == $prefix) {
                $toDelete[] = $key;
            }
        }

        if (!empty($toDelete)) {
            $this->connection->deleteMulti($toDelete);
        }
    }

    public function increment($key, $offset = 1, $initial = 0, $expTime = null)
    {
        $expTime = (int)$expTime;
        $expTime = ($expTime > 0)
            ? $expTime
            : $this->defaultExpireTime;

        $k = $this->key($key);
        $this->connection->add($k, $initial, $expTime);
        return $this->connection->increment($this->key($key), $offset, $initial, $expTime);
    }

    public function clear()
    {
        $this->connection->flush();
    }

    private function connect()
    {
        // WARNING: Avoid persistent connections from cron tasks: task runner uses fork
        // emulate persistent connection_id.
        $mcObject = new \Memcached($this->cacheNamespace . getmypid());

        if (!count($mcObject->getServerList())) {
            // WARNING: persistent connection settings must be set only once!
            $mcObject->setOptions([
                \Memcached::OPT_HASH => \Memcached::HASH_MURMUR,
                \Memcached::OPT_BINARY_PROTOCOL => true,
                \Memcached::OPT_PREFIX_KEY => $this->cacheNamespace,
                \Memcached::OPT_TCP_NODELAY => true //for small data packets
            ]);

            if (!$mcObject->addServer($this->host, $this->port)) {
                throw new \Exception("Connection to memcached: {$this->host}:{$this->port} failed");
            }
        }

        return $mcObject;
    }

    private function key($key, $glob = false)
    {
        if (is_array($key)) {
            $keyPart = $this->key(array_shift($key));
            $groupPart = $this->key(array_shift($key));
            return $groupPart . '_' . $keyPart;
        }
        return md5((string) $key) . ($glob ? '_' : '');
    }
}
