<?php

namespace PP;

use PP\Lib\Engine\AbstractEngine;
use PXApplication;
use Symfony\Component\Cache\Adapter\FilesystemAdapter;
use Symfony\Component\Finder\Finder;

/**
 * Class ApplicationFactory.
 *
 * @package PP
 */
class ApplicationFactory
{
    /**
     * Makes cache namespace out of engine instance.
     * Use this to instanciate your own cacher.
     *
     * @param AbstractEngine $engine
     * @return string
     */
    public static function makeEngineCacheNamespace(AbstractEngine $engine)
    {
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
    public static function getDefaultEngineCachePath()
    {
        return join(DIRECTORY_SEPARATOR, [CACHE_PATH, 'config']);
    }

    /**
     * Creates application instance.
     *
     * @param AbstractEngine $engine
     * @return PXApplication
     * @throws
     */
    public static function create(AbstractEngine $engine)
    {
        $namespace = static::makeEngineCacheNamespace($engine);
        $path = static::getDefaultEngineCachePath();
        $cache = new FilesystemAdapter($namespace, 0, $path);

        $applicationCacheItem = $cache->getItem(PXApplication::class);

        if ($applicationCacheItem->isHit()) {
            $cachedApplication = $applicationCacheItem->get();

            $paths = $cachedApplication->getConfigurationPaths();
            $created = $cachedApplication->getCreated();

            $finder = new Finder();
            $finder->files()
                ->ignoreUnreadableDirs()->ignoreDotFiles(false)
                ->name('*.{yml,yaml,xml,ini}')->name('.env')
                ->depth('== 0')
                ->date('>= @' . $created)
                ->in(BASEPATH)->in($paths);

            if (count($finder) === 0) {
                return $cachedApplication;
            }
        }

        $application = new PXApplication();
        $application->init();

        $applicationCacheItem->set($application);
        $cache->save($applicationCacheItem);

        return $application;
    }

}
