<?php

namespace PP\DependencyInjection\Compiler;

use Symfony\Component\DependencyInjection\Compiler\CompilerPassInterface;
use Symfony\Component\DependencyInjection\ContainerBuilder;
use Symfony\Component\DependencyInjection\Reference;

class AddLoggingHandlersPass implements CompilerPassInterface {

	public function process(ContainerBuilder $container) {
		if (!$container->has('logger')) {
			return;
		}

		$logger = $container->getDefinition('logger');
		$serviceIds = $container->findTaggedServiceIds('logger.handler');

		foreach ($serviceIds as $id => $tags) {
			$logger->addMethodCall('pushHandler', [new Reference($id)]);
		}
	}

}
