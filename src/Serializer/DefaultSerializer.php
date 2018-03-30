<?php

namespace PP\Serializer;

/**
 * Class DefaultSerializer.
 *
 * @package PP\Serializer
 */
class DefaultSerializer implements SerializerInterface {

	/**
	 * @return string
	 */
	public function getName() {
		return 'default';
	}

	/**
	 * @return bool
	 */
	public function isSupported() {
		return true;
	}

	/**
	 * @param $data
	 * @return string
	 */
	public function serialize($data) {
		return serialize($data);
	}

	/**
	 * @param $data
	 * @return mixed
	 */
	public function unserialize($data) {
		return @unserialize($data);
	}

}
