<?php

namespace PP\Lib\PersistentQueue;

use Exception;
use PXApplication;
use PXDatabase;

/**
 * Class Queue.
 *
 * @package PP\Lib\PersistentQueue
 */
class Queue
{
    /**
     * @var string
     */
    public const JOB_DB_TYPE = 'queue_job';

    /**
     * @var string
     */
    public const JOB_FETCH_LIMIT = 1;

    /**
     * @var PXApplication
     */
    protected $app;

    /**
     * @var PXDatabase
     */
    protected $db;

    /**
    * Queue constructor.
    *
    * @throws Exception
    */
    public function __construct(PXApplication $app, PXDatabase $db)
    {
        $this->app = $app;
        $this->db = $db;
        if (!$this->app->getDataType(static::JOB_DB_TYPE)) {
            throw new Exception(
                sprintf("Queue job fatal error: datatype `%s` missed in datatypes.xml", static::JOB_DB_TYPE)
            );
        }
    }

    /**
    * @return int
    */
    public function addJob(Job $job)
    {
        $contentType = $this->app->getDataType(static::JOB_DB_TYPE);
        $jobObject = $job->toArray();

        $stub = $this->app->initContentObject(static::JOB_DB_TYPE);
        $object = array_merge($stub, $jobObject);

        return $this->db->addContentObject($contentType, $object);
    }

    /**
    * @return null
    */
    protected function updateJob(Job $job)
    {
        $contentType = $this->app->getDataType(static::JOB_DB_TYPE);
        return $this->db->modifyContentObject($contentType, $job->toArray());
    }

    /**
  * @return null
  */
    public function finishJob(Job $job)
    {
        $job->setState(Job::STATE_FINISHED);
        return $this->updateJob($job);
    }

    /**
    * @return null
    */
    public function failJob(Job $job)
    {
        $job->setState(Job::STATE_FAILED);
        return $this->updateJob($job);
    }

    /**
    * @return null
    */
    public function startJob(Job $job)
    {
        $job->setState(Job::STATE_IN_PROGRESS);
        return $this->updateJob($job);
    }

    /**
     * @param int $limit
     * @return Job[]
     */
    public function getFreshJobs($limit = self::JOB_FETCH_LIMIT)
    {
        $contentType = $this->app->getDataType(static::JOB_DB_TYPE);
        $objects = $this->db->getObjectsByFieldLimited(
            $contentType,
            true,
            'state',
            Job::STATE_FRESH,
            $limit,
            0
        );

        return array_map(fn ($object) => Job::fromArray($object), $objects);
    }

}
