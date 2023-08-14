<?php

namespace PP;

use PP\Lib\Cache\CacheInterface;
use PXApplication;
use Symfony\Component\Cache\Adapter\FilesystemAdapter;
use Symfony\Component\Finder\Finder;
use Symfony\Contracts\Cache\ItemInterface;

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
		$namespace = strtolower($engine::class);
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
	 * @param mixed $cache
	 * @return PXApplication
	 * @throws
	 */
	public static function create($engine, CacheInterface $cache = null) {
		$namespace = static::makeEngineCacheNamespace($engine);
		$path = static::getDefaultEngineCachePath();
		$cache = $cache ?: new FilesystemAdapter($namespace, 0, $path);

		$cachedApplication = $cache->get(PXApplication::class, function (ItemInterface $item) {
			$application = new PXApplication();
			$application->init();

			return $application;
		});

		$paths = $cachedApplication->getConfigurationPaths();
		$created = $cachedApplication->getCreated();
		$finder = new Finder();
		$finder->files()
			->ignoreUnreadableDirs()->ignoreDotFiles(false)
			->name('*.{yml,yaml,xml,ini}')->name('.env')
			->depth('== 0')
			->date('>= @' . $created)
			->in(BASEPATH)->in($paths);

		return $cachedApplication;
	}

}
