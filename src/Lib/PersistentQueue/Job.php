<?php

namespace PP\Lib\PersistentQueue;

use PP\Lib\IArrayable;

/**
 * Class Job
 * @package PP\Lib\PersistentQueue
 */
class Job implements IArrayable {

	/**
	 * @var string
	 */
	const STATE_FRESH = 'fresh';

	/**
	 * @var string
	 */
	const STATE_FINISHED = 'finished';

	/**
	 * @var string
	 */
	const STATE_FAILED = 'failed';

	/**
	 * @var string
	 */
	const STATE_IN_PROGRESS = 'in progress';

	/**
	 * @var int
	 */
	private $id = 0;

	/**
	 * @var array
	 */
	private $payload;

	/**
	 * @var string
	 */
	private $worker;

	/**
	 * @var string
	 */
	private $state;

	/**
	 * Job constructor
	 */
	public function __construct() {
		$this->state = static::STATE_FRESH;
	}

	/**
	 * Converts instance to array
	 *
	 * @return array
	 */
	public function toArray() {
		return [
			'id' => $this->getId(),
			'worker' => $this->getWorker(),
			'payload' => $this->getPayload(),
			'state' => $this->getState()
		];
	}

	/**
	 * Creates instance from array
	 *
	 * @param array $object
	 * @return static
	 */
	public static function fromArray(array $object) {
		$job = new static;
		$job->setId(getFromArray($object, 'id', 0));
		$job->setState(getFromArray($object, 'state', static::STATE_FRESH));
		$job->setPayload($object['payload']);
		$job->setWorker($object['worker']);

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
	 * @param string $worker
	 * @return $this
	 */
	public function setWorker($worker) {
		$this->worker = $worker;

		return $this;
	}

	/**
	 * @return string
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
	 * @return int
	 */
	public function getId() {
		return $this->id;
	}

	/**
	 * @param int $id
	 */
	public function setId($id) {
		$this->id = $id;

		return $this;
	}

}
