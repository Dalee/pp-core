<?php

namespace PP\Lib\Xml;

/**
 * Interface XmlInterface
 * @package PP\Lib\Xml
 */
interface XmlInterface
{

	/**
	 * @param string $query
	 * @return XmlNodeInterface[]
	 */
	public function xpath($query);
}
