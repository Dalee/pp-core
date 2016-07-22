<?php

namespace PP\Lib\PersistentQueue;

use PXRegistry;

/**
 * Class Queue
 * @package PP\Lib\PersistentQueue
 */
class Queue {

	const JOB_DB_TYPE = 'queue_job';

	/**
	 * @var \PXApplication
	 */
	private $app;

	/**
	 * @var \PXDatabase
	 */
	private $db;

	/**
	 * Queue constructor
	 */
	public function __construct() {
		$this->app = PXRegistry::getApp();
		$this->db = PXRegistry::getDb();
	}

	/**
	 * @param Job $job
	 * @return int
	 */
	public function addJob(Job $job) {
		$contentType = $this->app->types[static::JOB_DB_TYPE];
		$jobObject = $job->toArray();

		$stub = $this->app->initContentObject($contentType);
		$object = array_merge($stub, $jobObject);

		return $this->db->addContentObject($contentType, $object);
	}

	/**
	 * @return Job[]
	 */
	public function getFreshJobs() {
		$contentType = $this->app->types[static::JOB_DB_TYPE];
		$objects = $this->db->getObjectsByField(
			$contentType, true,
			'state', Job::STATE_FRESH
		);

		return array_map(function ($object) {
			return Job::fromArray($object);
		}, $objects);
	}

}
