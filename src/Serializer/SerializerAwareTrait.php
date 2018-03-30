<?php

namespace PP\Serializer;

/**
 * Trait SerializerAwareTrait.
 *
 * @package PP\Serializer
 */
trait SerializerAwareTrait {

	/**
	 * @var SerializerInterface
	 */
	protected $serializer;

	/**
	 * @param SerializerInterface $serializer
	 */
	public function setSerializer(SerializerInterface $serializer) {
		$this->serializer = $serializer;
	}
}
