<?php

namespace PP\DependencyInjection;

use Symfony\Component\Config\Loader\LoaderInterface;
use Symfony\Component\DependencyInjection\ContainerBuilder;
use Symfony\Component\DependencyInjection\Extension\Extension;
use Symfony\Component\DependencyInjection\Loader\YamlFileLoader;
use Symfony\Component\Config\FileLocator;

/**
 * Class CoreExtension.
 *
 * @package PP\DependencyInjection
 */
class CoreExtension extends Extension {

	/**
	 * @inheritdoc
	 * @throws \Exception
	 */
	public function load(array $configs, ContainerBuilder $container) {
		$loader = new YamlFileLoader($container, new FileLocator(PPSERVICESPATH));

		$configuration = $this->getConfiguration($configs, $container);
		$config = $this->processConfiguration($configuration, $configs);

		$container->setParameter('core.base_dir', BASEPATH);
		$container->setParameter('core.app_dir', APPPATH);
		$container->setParameter('core.cache_dir', CACHE_PATH);
		$container->setParameter('core.runtime_dir', RUNTIME_PATH);

		// register event dispatcher configuration
		$loader->load('event_dispatcher.yml');

		$this->registerLoggerConfiguration($config['application'], $container, $loader);
	}

	/**
  * @param string $applicationName
  * @throws \Exception
  */
 private function registerLoggerConfiguration($applicationName, ContainerBuilder $container, LoaderInterface $loader) {
		$loader->load('logger.yml');
		$container->getDefinition('logger')->addArgument($applicationName);
	}

	/**
	 * @inheritdoc
	 */
	public function getAlias(): string
	{
		return 'core';
	}

}
