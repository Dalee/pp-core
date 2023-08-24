<?php

namespace PP\Lib\Cache\Driver;

use PP\Lib\Cache\CacheInterface;

/**
 * Class Apc
 * @package PP\Lib\Cache\Driver
 */
class Apc implements CacheInterface
{
    private readonly string $cacheDomain;
    private readonly int $expirationTime;

    public function __construct($cacheDomain = null, $defaultExpire = 3600)
    {
        extension_loaded("apc") && ini_get("apc.enabled") or FatalError(static::class . " error: APC extension disabled or doesn't installed");
        $this->expirationTime = (int)$defaultExpire;
        $this->cacheDomain = BASEPATH . $cacheDomain;
    }

    private function key($key, $glob = false)
    {
        if (is_array($key)) {
            $keyPart = $this->key(array_shift($key));
            $groupPart = $this->key(array_shift($key));
            return $groupPart . '_' . $keyPart;
        }
        return !$glob ? md5($this->cacheDomain . $key) : '#^' . md5($this->cacheDomain . $key) . '_\w+$#';
    }

    public function exists($key)
    {
        apc_fetch($this->key($key), $success);
        return $success;
    }

    public function save($key, $data, $expTime = null)
    {
        apc_store($this->key($key), $data, ((int)$expTime ?: $this->expirationTime)) or $this->clear();
    }

    public function load($key)
    {
        $res = apc_fetch($this->key($key), $success);
        return $success ? $res : null;
    }

    public function delete($key)
    {
        return apc_delete($this->key($key));
    }

    public function increment($key, $offset = 1, $initial = 0, $expTime = null)
    {
        $key = $this->key($key);
        apc_exists($key) || apc_store($key, $initial, (int)$expTime);
        return apc_inc($key, $offset);
    }

    public function clear()
    {
        return apc_clear_cache("user");
    }

    public function deleteGroup($group)
    {
        foreach (new \APCIterator('user', $this->key($group, true), APC_ITER_KEY) as $key) {
            apc_delete($key);
        }
    }
}
