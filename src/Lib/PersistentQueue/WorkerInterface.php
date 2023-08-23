<?php

namespace PP\Lib\PersistentQueue;

/**
 * Interface WorkerInterface.
 *
 * @package PP\Lib\PersistentQueue
 */
interface WorkerInterface
{
    /**
    * @return mixed
    */
    public function run(array $payload = []);

    /**
    * @return $this
    */
    public function setJob(Job $job);

}
