<?php

namespace PP;

use PXApplication;
use Psr\SimpleCache\CacheInterface;
use Symfony\Component\Cache\Simple\FilesystemCache;

/**
 * Class ApplicationCacheFactory.
 *
 * @package PP
 */
class ApplicationCacheFactory {

	/**
	 * Makes cache key out of engine instance.
	 * Use this to instanciate your own cacher.
	 *
	 * @param \PP\Lib\Engine\AbstractEngine $engine
	 * @return string
	 */
	public static function makeEngineCacheKey($engine) {
		return strtolower(get_class($engine));
	}

	/**
	 * Returns default cache path.
	 * Use this to instanciate your own cacher.
	 *
	 * @return string
	 */
	public static function getDefaultEngineCachePath() {
		return join(DIRECTORY_SEPARATOR, [CACHE_PATH, 'config']);
	}

	/**
	 * Creates or loads application instance.
	 *
	 * @param \PP\Lib\Engine\AbstractEngine $engine
	 * @param CacheInterface|null $cache
	 * @return PXApplication
	 */
	public static function create($engine, CacheInterface $cache = null) {
		$engineClass = get_class($engine);
		$cacheKey = static::makeEngineCacheKey($engine);
		$cachePath = static::getDefaultEngineCachePath();
		$cache = $cache ?: new FilesystemCache($cacheKey, 0, $cachePath);

		if ($cachedApplication = $cache->get($cacheKey)) {
			$paths = $cachedApplication->getConfigurationPaths();
			$created = $cachedApplication->getCreated();
			$reinit = false;

			foreach ($paths as $path) {
				$d = new NLDir($path);

				while ($entry = $d->ReadFull()) {
					$tmp = stat($entry);

					if ($tmp['mtime'] >= $created) {
						$reinit = true;
						break;
					}
				}

				if ($reinit) {
					$cache->delete($cacheKey);
					break;
				}
			}

			if (!$reinit) {
				return $cachedApplication;
			}
		}

		$application = new PXApplication($engineClass, $engine);

		MakeDirIfNotExists($cachePath);
		$cache->set($cacheKey, $application);

		return $application;
	}

}
