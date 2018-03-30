<?php

namespace PP\Lib\Cache\Driver;

use PP\Lib\Cache\CacheInterface;

/**
 * Class Null
 * @package PP\Lib\Cache\Driver
 */
class NullCache implements CacheInterface {
	function exists($objectKey) {
	}

	function save($objectKey, $dataForSave, $expirationTime = null) {
	}

	function load($objectKey) {
	}

	function delete($objectKey) {
	}

	function clear() {
	}

	function increment($key, $offset = 1, $initial = 0, $expTime = null) {
		return $initial;
	}

	function deleteGroup($group) {
		return true;
	}
}
