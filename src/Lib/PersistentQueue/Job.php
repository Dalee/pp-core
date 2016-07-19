<?php

namespace PP\Lib\PersistentQueue;

use DateTime;

/**
 * Class Job
 * @package PP\Lib\PersistentQueue
 */
class Job {

	/**
	 * @var string
	 */
	const STATE_FRESH = 'fresh';

	/**
	 * @var array
	 */
	private $payload;

	/**
	 * @var IWorker
	 */
	private $worker;

	/**
	 * @var DateTime
	 */
	private $created;

	/**
	 * @var string
	 */
	private $state;

	/**
	 * Job constructor
	 */
	public function __construct() {
		$this->created = new DateTime;
		$this->state = static::STATE_FRESH;
	}

	public static function fromArray(array $object) {
		$job = new static;
		$job->setState($object['state']);
		// TODO: double check if it's a string
		$job->setPayload($object['payload']);
		// TODO: need a resolver
		// $job->setWorker();
		// TODO: double check it's ok
		$job->setCreated(new DateTime($object['created']));

		return $job;
	}

	/**
	 * @param array $payload
	 * @return $this
	 */
	public function setPayload(array $payload) {
		$this->payload = $payload;

		return $this;
	}

	/**
	 * @return array
	 */
	public function getPayload() {
		return $this->payload;
	}

	/**
	 * @param IWorker $worker
	 * @return $this
	 */
	public function setWorker(IWorker $worker) {
		$this->worker = $worker;

		return $this;
	}

	/**
	 * @return IWorker
	 */
	public function getWorker() {
		return $this->worker;
	}

	/**
	 * @return string
	 */
	public function getState() {
		return $this->state;
	}

	/**
	 * @param $state
	 * @return $this
	 */
	public function setState($state) {
		$this->state = $state;

		return $this;
	}

	/**
	 * @param DateTime $created
	 * @return $this
	 */
	public function setCreated(DateTime $created) {
		$this->created = $created;

		return $this;
	}

	/**
	 * @return DateTime
	 */
	public function getCreated() {
		return $this->created;
	}

}
