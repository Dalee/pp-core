<?php

namespace PP\DependencyInjection;

use Symfony\Component\Config\Definition\ConfigurationInterface;
use Symfony\Component\Config\Definition\Builder\TreeBuilder;

/**
 * Class Configuration.
 *
 * @package PP\DependencyInjection
 */
class Configuration implements ConfigurationInterface {

	public function getConfigTreeBuilder() {
		$tree = new TreeBuilder('core');
		$rootNode = $tree->getRootNode();

		$rootNode->children()
			->scalarNode('application')->defaultValue('app')->end()
		->end();

		return $tree;
	}

}
