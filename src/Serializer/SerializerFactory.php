<?php

namespace PP\Serializer;

/**
 * Class SerializerFactory.
 *
 * @package PP\Serializer
 */
class SerializerFactory {

	/**
	 * @param $driver
	 * @return SerializerInterface
	 */
	public static function create($driver) {
		$driver = ucfirst(strtolower($driver));
		$class = 'PP\Serializer\\' . $driver . 'Serializer';

		if (!class_exists($class)) {
			FatalError("Serializer class {$class} not found");
		}

		/** @var SerializerInterface $instance */
		$instance = new $class();
		if (!$instance->isSupported()) {
			FatalError("Serializer instance of {$class} is not supported");
		}

		return $instance;
	}

}
