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
	 * @return $this
	 */
	public function addJob(Job $job) {
		//

		return $this;
	}

	/**
	 * @return Job[]
	 */
	public function getFreshJobs() {
		$objects = $this->db->getObjectsByField(
			$this->app->types[static::JOB_DB_TYPE], true,
			'state', Job::STATE_FRESH
		);

		return array_map(function ($object) {
			return Job::fromArray($object);
		}, $objects);
	}

}
