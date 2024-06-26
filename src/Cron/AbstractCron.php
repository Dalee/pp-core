<?php

namespace PP\Cron;

use PP\Lib\Datastruct\Tree;
use PP\DependencyInjection\ContainerAwareInterface;
use PP\DependencyInjection\ContainerAwareTrait;
use PP\Lib\Database\Driver\PostgreSqlDriver;

/**
 * Class AbstractCron.
 *
 * @package PP\Cron
 */
abstract class AbstractCron implements ContainerAwareInterface
{
    use ContainerAwareTrait;

    /**
     * TODO: should be protected
     * @var string
     */
    public $name = 'Abstract CronRun Class';

    /**
  * @param \PXApplication $app
  * @param Tree $tree
  * @param int $matchedTime
  * @param CronRule $matchedRule
  * @return array
  */
    public function Run($app, \PXDatabase|\PP\Lib\Database\Driver\PostgreSqlDriver $db, $tree, $matchedTime, $matchedRule)
    {
        return [
            'status' => -1,
            'note' => 'Не определен метод Run()'
        ];
    }

    /**
     * @return string
     */
    public function getName()
    {
        return $this->name;
    }

    protected function log($message)
    {
        \PXRegistry::getLogger(LOGGER_CRON)->info($message);
    }

    protected function error($message)
    {
        \PXRegistry::getLogger(LOGGER_CRON)->error($message);
    }
}
