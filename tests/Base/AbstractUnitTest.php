<?php

namespace Tests\Base;

/**
 * Class AbstractUnitTest
 * @package Tests\Base
 */
abstract class AbstractUnitTest extends AbstractApplicationTest {

	/**
	 * @param $object
	 * @param string $name
	 * @param $value
	 */
	protected function setProtectedProperty($object, $name, $value) {
		$reflection = new \ReflectionClass($object);
		$reflectionProperty = $reflection->getProperty($name);
		$reflectionProperty->setAccessible(true);
		$reflectionProperty->setValue($object, $value);
	}

	/**
	 * @param $object
	 * @param string $name
	 * @return mixed
	 */
	protected function getProtectedProperty($object, $name) {
		$reflection = new \ReflectionClass($object);
		$reflectionProperty = $reflection->getProperty($name);
		$reflectionProperty->setAccessible(true);
		return $reflectionProperty->getValue($object);
	}

	/**
	 * @param $object
	 * @param string $method
	 * @param array $params
	 * @return mixed
	 */
	protected function callProtectedMethod($object, $method, array $params) {
		$reflection = new \ReflectionClass($object);
		$reflectionMethod = $reflection->getMethod($method);
		$reflectionMethod->setAccessible(true);
		return $reflectionMethod->invokeArgs($object, $params);
	}

}
