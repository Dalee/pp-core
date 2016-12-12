<?php

namespace PP\Lib\Xml;

/**
 * Interface XmlInterface
 * @package PP\Lib\Xml
 */
interface XmlInterface {

	/**
	 * @param string $query
	 * @return XmlNodeInterface[]
	 */
	function xpath($query);
}
