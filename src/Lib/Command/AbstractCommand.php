<?php

namespace PP\Lib\Command;

use PP\DependencyInjection\ContainerAwareInterface;
use PP\DependencyInjection\ContainerAwareTrait;
use PP\Lib\Database\Driver\PostgreSqlDriver;

/**
 * Class AbstractCommand
 * @package PP\Lib\Command
 */
abstract class AbstractCommand extends AbstractBasicCommand implements ContainerAwareInterface
{
    use ContainerAwareTrait;

    /** @var \PXApplication */
    protected $app;

    /** @var \PXDatabase|PostgreSqlDriver */
    protected $db;

    /**
  * @return $this
  */
    public function setApp(\PXApplication $app)
    {
        $this->app = $app;

        return $this;
    }

    /**
  * @return $this
  */
    public function setDb(\PXDatabase $db)
    {
        $this->db = $db;

        return $this;
    }

}
