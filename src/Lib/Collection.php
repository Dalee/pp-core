<?php

namespace PP\Lib;

use Closure;
use Countable;
use IteratorAggregate;
use ArrayIterator;

/**
 * Class Collection
 * @package PP\Lib
 */
class Collection implements Countable, IteratorAggregate {

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
	 * @return Collection
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
	 * @return ArrayIterator
	 */
	public function getIterator() {
		return new ArrayIterator($this->elements);
	}

}
