<?php

namespace PP\Lib\Cache\Driver;

use PP\Lib\Cache\CacheInterface;

/**
 * Class Null
 * @package PP\Lib\Cache\Driver
 */
class NullCache implements CacheInterface
{
    public function exists($objectKey)
    {
    }

    public function save($objectKey, $dataForSave, $expirationTime = null)
    {
    }

    public function load($objectKey)
    {
    }

    public function delete($objectKey)
    {
    }

    public function clear()
    {
    }

    public function increment($key, $offset = 1, $initial = 0, $expTime = null)
    {
        return $initial;
    }

    public function deleteGroup($group)
    {
        return true;
    }
}
