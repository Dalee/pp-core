<?php

namespace PP;

use PXApplication;
use Psr\SimpleCache\CacheInterface;
use Symfony\Component\Cache\Simple\FilesystemCache;
use Symfony\Component\Finder\Finder;

/**
 * Class ApplicationFactory.
 *
 * @package PP
 */
class ApplicationFactory {

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
	 * Creates application instance.
	 * The default cache type is FilesystemCache.
	 *
	 * @param \PP\Lib\Engine\AbstractEngine $engine
	 * @param CacheInterface|null $cache
	 * @return PXApplication
	 */
	public static function create($engine, CacheInterface $cache = null) {
		$cacheKey = static::makeEngineCacheKey($engine);
		$cachePath = static::getDefaultEngineCachePath();
		$cache = $cache ?: new FilesystemCache($cacheKey, 0, $cachePath);

		if ($cachedApplication = $cache->get($cacheKey)) {
			$paths = $cachedApplication->getConfigurationPaths();
			$created = $cachedApplication->getCreated();
			$finder = new Finder();
			$finder->files()
				->ignoreUnreadableDirs()
				->depth('== 0')
				->date('>= @' . $created)
				->in($paths);

			if (count($finder) === 0) {
				return $cachedApplication;
			}

			$cache->delete($cacheKey);
		}

		$application = new PXApplication($engine);
		$cache->set($cacheKey, $application);

		return $application;
	}

}
