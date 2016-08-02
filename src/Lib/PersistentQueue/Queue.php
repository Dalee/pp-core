<?php

namespace PP\Lib\PersistentQueue;

use PXApplication;
use PXDatabase;

/**
 * Class Queue
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
	private $app;

	/**
	 * @var PXDatabase
	 */
	private $db;

	/**
	 * @var array
	 */
	private $workers = [];

	/**
	 * Queue constructor
	 */
	public function __construct(PXApplication $app, PXDatabase $db) {
		$this->app = $app;
		$this->db = $db;

		$directory = $app->directory['content-generators'];
		$directory->loaded || $db->loadDirectory($directory, null);
		$this->workers = $directory->values;
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

	/**
	 * @param string $id
	 * @return mixed|null
	 */
	public function getWorkerClassByID($id) {
		if (!isset($this->workers[$id]['class'])) {
			return null;
		}

		return $this->workers[$id]['class'];
	}

	/**
	 * @return array
	 */
	public function getWorkers() {
		return $this->workers;
	}

	/**
	 * @param WorkerInterface $worker
	 */
	public function instanciateWorkerForJob(Job $job) {
		$worker = $job->getWorker();
		$workerClass = $this->getWorkerClassByID($worker);

		if (!$workerClass || !class_exists($workerClass)) {
			throw new \Exception;
		}

		$interfaces = class_implements($workerClass);
		// miss you 5.4 - WorkerInterface::class
		if (!isset($interfaces['PP\Lib\PersistentQueue\WorkerInterface'])) {
			throw new \Exception;
		}

		return new $workerClass($this->app, $this->db);
	}

}
