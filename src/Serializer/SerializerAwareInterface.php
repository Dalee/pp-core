<?php

namespace PP\Serializer;

/**
 * Interface SerializerAwareInterface.
 *
 * @package PP\Serializer
 */
interface SerializerAwareInterface {

	/**
	 * @param SerializerInterface $serializer
	 */
	public function setSerializer(SerializerInterface $serializer);

}
