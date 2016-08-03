<?php

namespace PP\Lib;

/**
 * Interface IArrayable
 * @package PP\Lib
 */
interface IArrayable {

	/**
	 * Converts instance to array
	 *
	 * @return mixed
	 */
	public function toArray();

	/**
	 * Creates instance from array
	 *
	 * @param array $object
	 * @return mixed
	 */
	public static function fromArray(array $object);

}
