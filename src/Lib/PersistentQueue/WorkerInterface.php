<?php

namespace PP\Lib\PersistentQueue;

/**
 * Interface WorkerInterface
 * @package PP\Lib\PersistentQueue
 */
interface WorkerInterface {

	/**
	 * @return string
	 */
	public function getName();

	/**
	 *
	 */
	public function work();

}
