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
	 * Makes cache namespace out of engine instance.
	 * Use this to instanciate your own cacher.
	 *
	 * @param \PP\Lib\Engine\AbstractEngine $engine
	 * @return string
	 */
	public static function makeEngineCacheNamespace($engine) {
		$namespace = strtolower(get_class($engine));
		$namespace = str_replace('\\', '', $namespace);

		return $namespace;
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
		$namespace = static::makeEngineCacheNamespace($engine);
		$path = static::getDefaultEngineCachePath();
		$cache = $cache ?: new FilesystemCache($namespace, 0, $path);

		if ($cachedApplication = $cache->get(PXApplication::class)) {
			$paths = $cachedApplication->getConfigurationPaths();
			$created = $cachedApplication->getCreated();
			$finder = new Finder();
			$finder->files()
				->ignoreUnreadableDirs()->ignoreDotFiles(false)
				->name('*.{yml,xml,ini}')->name('.env')
				->depth('== 0')
				->date('>= @' . $created)
				->in(BASEPATH)->in($paths);

			if (count($finder) === 0) {
				return $cachedApplication;
			}
		}

		$application = new PXApplication($engine);
		$cache->set(PXApplication::class, $application);

		return $application;
	}

}
