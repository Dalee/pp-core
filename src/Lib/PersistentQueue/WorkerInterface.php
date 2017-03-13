<?php

namespace PP\Lib\PersistentQueue;

/**
 * Interface WorkerInterface.
 *
 * @package PP\Lib\PersistentQueue
 */
interface WorkerInterface {

	/**
	 * @param array $payload
	 * @return mixed
	 */
	public function run(array $payload = []);

	/**
	 * @param Job $job
	 * @return $this
	 */
	public function setJob(Job $job);

}
