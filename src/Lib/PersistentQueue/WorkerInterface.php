<?php

namespace PP\Lib\PersistentQueue;

/**
 * Interface WorkerInterface
 * @package PP\Lib\PersistentQueue
 */
interface WorkerInterface {

	/**
	 *
	 */
	public function run(array $params = []);

}
