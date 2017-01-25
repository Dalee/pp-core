<?php

namespace PP\Lib\Objects;

/**
 * Interface ContentObjectsInterface
 * @package PP\Lib\Objects
 */
interface ContentObjectsInterface {

	function hasCurrent();

	function getCurrent();

	function getAllowedChilds();

	/**
	 * @param string $type
	 * @return bool
	 */
	function hasType($type);

	function getCurrentType();

	function getLinks();
}
