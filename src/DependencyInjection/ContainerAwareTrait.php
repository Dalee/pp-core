<?php

namespace PP\DependencyInjection;

use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * ContainerAware trait.
 * @author Fabien Potencier <fabien@symfony.com>
 */
trait ContainerAwareTrait
{
    /**
     * @var ContainerInterface|null
     */
    protected $container;


    /**
     * @param ContainerInterface|null $container
     * @return void
     */
    public function setContainer(?ContainerInterface $container = null): void
    {
        if (1 > \func_num_args()) {
            trigger_deprecation('symfony/dependency-injection', '6.2', 'Calling "%s::%s()" without any arguments is deprecated, pass null explicitly instead.', __CLASS__, __FUNCTION__);
        }

        $this->container = $container;
    }
}
