<?php

namespace PP\Lib\PersistentQueue;

use \PXApplication;
use \PXDatabase;

/**
 * Class Queue.
 *
 * @package PP\Lib\PersistentQueue
 */
class Queue {

	/**
	 * @var string
	 */
	const JOB_DB_TYPE = 'queue_job';

	/**
	 * @var string
	 */
	const JOB_FETCH_LIMIT = 1;

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
	 * @param PXApplication $app
	 * @param PXDatabase $db
	 */
	public function __construct(PXApplication $app, PXDatabase $db) {
		$this->app = $app;
		$this->db = $db;
	}

	/**
	 * @param Job $job
	 * @return int
	 */
	public function addJob(Job $job) {
		$contentType = $this->app->types[static::JOB_DB_TYPE];
		$jobObject = $job->toArray();

		$stub = $this->app->initContentObject(static::JOB_DB_TYPE);
		$object = array_merge($stub, $jobObject);

		return $this->db->addContentObject($contentType, $object);
	}

	/**
	 * @param Job $job
	 * @return null
	 */
	protected function updateJob(Job $job) {
		$contentType = $this->app->types[static::JOB_DB_TYPE];
		return $this->db->ModifyContentObject($contentType, $job->toArray());
	}

	/**
	 * @param Job $job
	 * @return null
	 */
	public function finishJob(Job $job) {
		$job->setState(Job::STATE_FINISHED);
		return $this->updateJob($job);
	}

	/**
	 * @param Job $job
	 * @return null
	 */
	public function failJob(Job $job) {
		$job->setState(Job::STATE_FAILED);
		return $this->updateJob($job);
	}

	/**
	 * @param Job $job
	 * @return null
	 */
	public function startJob(Job $job) {
		$job->setState(Job::STATE_IN_PROGRESS);
		return $this->updateJob($job);
	}

	/**
	 * @param int $limit
	 * @return Job[]
	 */
	public function getFreshJobs($limit = self::JOB_FETCH_LIMIT) {
		$contentType = $this->app->types[static::JOB_DB_TYPE];
		$objects = $this->db->getObjectsByFieldLimited(
			$contentType, true,
			'state', Job::STATE_FRESH,
			$limit, 0
		);

		return array_map(function ($object) {
			return Job::fromArray($object);
		}, $objects);
	}

}
