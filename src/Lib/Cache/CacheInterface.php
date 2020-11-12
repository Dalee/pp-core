<?php

namespace PP\Lib\Cache;

/**
 * Interface CacheInterface
 * @package PP\Lib\Cache
 */
interface CacheInterface
{
    public function exists($objectKey);

    public function save($objectKey, $dataForSave, $expirationTime = null);

    public function load($objectKey);

    public function delete($objectKey);

    public function clear();

    public function increment($numberKey, $offset = 1, $initial = 0, $expirationTime = null);

    public function deleteGroup($group);
}
