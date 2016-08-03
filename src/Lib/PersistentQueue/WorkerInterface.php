<?php

namespace PP\Lib\PersistentQueue;

/**
 * Interface WorkerInterface
 * @package PP\Lib\PersistentQueue
 */
interface WorkerInterface {

	/**
	 * Fires worker
	 */
	public function run(array $params = []);

}
