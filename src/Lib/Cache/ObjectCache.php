<?php

namespace PP\Lib\Cache;

/**
 *
 * Class ObjectCache
 * @package PP\Lib\Cache
 */
class ObjectCache {

	/**
	 * Cache definition can contains extra args string for concrete engine after its name@
	 * Now cache definition is URL scheme base, with backward compatibility
	 *
	 * @param string $cache
	 * @param null $cacheDomain
	 * @param int $defaultExpire
	 * @return CacheInterface
	 * @throws \Exception
	 */
	public static function get($cache, $cacheDomain = null, $defaultExpire = 3600) {
		$extraArgs = null;
		$cacheClass = '';

		switch (true) {
			case $cache === '1':
			case $cache === 'file':
			case $cache === true:
				$cacheClass = 'PP\Lib\Cache\Driver\File';
				break;

			case $cache === '':
			case $cache === 'null':
			case $cache === null:
			case $cache === false:
				$cacheClass = 'PP\Lib\Cache\Driver\NullCache';
				break;

			case strpos($cache, '://') !== false:
				$extraArgs = parse_url($cache);
				$cacheClass = 'PP\Lib\Cache\Driver\\' . ucfirst($extraArgs['scheme']);
				break;

			case strpos($cache, '@') !== false:
				list($cache, $extraArgs) = explode('@', $cache, 2);
				$cacheClass = 'PP\Lib\Cache\Driver\\' . ucfirst($cache);
				break;
		}

		if (!class_exists($cacheClass)) {
			FatalError("Caching method: '{$cache}'/'{$cacheClass}' - is not implemented");
		}

		$instance = new $cacheClass($cacheDomain, $defaultExpire, $extraArgs);
		if (!$instance instanceof CacheInterface) {
			FatalError("Caching method:'{$cache}' - doesn't follow CacheInterface");
		}

		return $instance;
	}
}
