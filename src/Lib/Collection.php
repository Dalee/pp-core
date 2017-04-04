<?php

namespace PP\Lib;

use Closure;
use Countable;
use IteratorAggregate;
use ArrayIterator;
use JsonSerializable;

/**
 * Class Collection
 * @package PP\Lib
 */
class Collection implements Countable, IteratorAggregate, JsonSerializable {

	/**
	 * @var array
	 */
	protected $elements = [];

	/**
	 * Collection constructor.
	 * @param array $elements
	 */
	public function __construct(array $elements = []) {
		$this->elements = $elements;
	}

	/**
	 * @return array
	 */
	public function toArray() {
		return $this->elements;
	}

	/**
	 * @return bool
	 */
	public function isEmpty() {
		return empty($this->elements);
	}

	/**
	 * @return int
	 */
	public function count() {
		return count($this->elements);
	}

	/**
	 * @param Closure $func
	 * @return array
	 */
	public function map(Closure $func) {
		return array_map($func, $this->elements);
	}

	/**
	 * @param Closure $func
	 * @return Collection
	 */
	public function filter(Closure $func) {
		return new static(array_filter($this->elements, $func));
	}

	/**
	 * @return mixed
	 */
	public function first() {
		return reset($this->elements);
	}

	/**
	 * @return mixed
	 */
	public function last() {
		return end($this->elements);
	}

	/**
	 * @return ArrayIterator
	 */
	public function getIterator() {
		return new ArrayIterator($this->elements);
	}

	/**
	 * @param  mixed  $key
	 * @return bool
	 */
	public function containsKey($key) {
		return array_key_exists($key, $this->elements);
	}

	/**
	 * @param  mixed  $key
	 * @param  mixed  $default
	 * @return mixed
	 */
	public function get($key, $default = null) {
		if ($this->containsKey($key)) {
			return $this->elements[$key];
		}

		return $default;
	}

	/**
	 * @param  mixed  $key
	 * @param  mixed  $value
	 * @return Collection
	 */
	public function set($key, $value) {
		if (is_null($key)) {
			$this->elements[] = $value;
		} else {
			$this->elements[$key] = $value;
		}

		return $this;
	}

	/**
	 * @param  mixed  $value
	 * @return Collection
	 */
	public function push($value) {
		$this->set(null, $value);

		return $this;
	}

	/**
	 * @inheritdoc
	 */
	public function jsonSerialize() {
		return $this->toArray();
	}

}
