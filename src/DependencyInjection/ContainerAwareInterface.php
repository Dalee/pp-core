<?php

namespace PP\DependencyInjection;

use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * ContainerAwareInterface should be implemented by classes that depends on a Container.
 *
 * @author Fabien Potencier <fabien@symfony.com>
 */
interface ContainerAwareInterface
{
	/**
	 * @param ContainerInterface|null $container
	 * @return void
	 */
    public function setContainer(?ContainerInterface $container);
}
