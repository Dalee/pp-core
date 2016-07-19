<?php

namespace PP\Lib\PersistentQueue;

/**
 * Interface IWorker
 * @package PP\Lib\PersistentQueue
 */
interface IWorker {

	/**
	 * @return string
	 */
	public function getName();

	/**
	 * 
	 */
	public function work();

}
