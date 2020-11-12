<?php

namespace PP\Lib\PersistentQueue;

use \ArrayIterator;
use PP\Lib\IArrayable;

/**
 * Class JobResult.
 *
 * @package PP\Lib\PersistentQueue
 */
class JobResult implements IArrayable {

	/**
	 * @var string
	 */
	public const RESULT_ERRORS = 'errors';

	/**
	 * @var string
	 */
	public const RESULT_INFO = 'info';

	/**
	 * @var string
	 */
	public const RESULT_NOTICES = 'notices';

	/**
	 * @var ArrayIterator
	 */
	protected $errors;

	/**
	 * @var ArrayIterator
	 */
	protected $info;

	/**
	 * @var ArrayIterator
	 */
	protected $notices;

	/**
	 * JobResult constructor.
	 */
	public function __construct() {
		$this->errors = new ArrayIterator();
		$this->info = new ArrayIterator();
		$this->notices = new ArrayIterator();
	}

	/**
	 * @param $message
	 * @return $this
	 */
	public function addError($message) {
		$this->errors->append($message);

		return $this;
	}

	/**
	 * @param $message
	 * @return $this
	 */
	public function addInfo($message) {
		$this->info->append($message);

		return $this;
	}

	/**
	 * @param $message
	 * @return $this
	 */
	public function addNotice($message) {
		$this->notices->append($message);

		return $this;
	}

	/**
	 * @param array $errors
	 * @return $this
	 */
	public function setErrors(array $errors) {
		$this->errors = new ArrayIterator($errors);

		return $this;
	}

	/**
	 * @param array $info
	 * @return $this
	 */
	public function setInfo(array $info) {
		$this->info = new ArrayIterator($info);

		return $this;
	}

	/**
	 * @param array $notices
	 * @return $this
	 */
	public function setNotices(array $notices) {
		$this->notices = new ArrayIterator($notices);

		return $this;
	}

	/**
	 * @inheritdoc
	 */
	public function toArray() {
		return [
			static::RESULT_ERRORS => $this->errors->getArrayCopy(),
			static::RESULT_INFO => $this->info->getArrayCopy(),
			static::RESULT_NOTICES => $this->notices->getArrayCopy()
		];
	}

	/**
	 * @inheritdoc
	 */
	public static function fromArray(array $object) {
		/** @var JobResult $instance */
		$instance = new static();

		return $instance
			->setErrors(getFromArray($object, static::RESULT_ERRORS, []))
			->setInfo(getFromArray($object, static::RESULT_INFO, []))
			->setNotices(getFromArray($object, static::RESULT_NOTICES, []));
	}

	/**
	 * @return ArrayIterator
	 */
	public function getErrors() {
		return $this->errors;
	}

	/**
	 * @return ArrayIterator
	 */
	public function getInfo() {
		return $this->info;
	}

	/**
	 * @return ArrayIterator
	 */
	public function getNotices() {
		return $this->notices;
	}

}
