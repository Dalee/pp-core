<?php

namespace PP\Lib\Xml;

/**
 * Interface XmlNodeInterface
 * @package PP\Lib\Xml
 */
interface XmlNodeInterface {

	/**
	 * @return string
	 */
	function nodeName();

	/**
	 * @return string
	 */
	function nodeValue();

	/**
	 * @param $xpath
	 * @return string
	 */
	function nodeXValue($xpath);

	/**
	 * @return mixed
	 */
	function nodeType();

	/**
	 * @return array
	 */
	function attributes();

	/**
	 * @param string $attrName
	 * @return mixed
	 */
	function getAttribute($attrName);

	/**
	 * @return XmlNodeInterface[]
	 */
	function childNodes();

	/**
	 * @param string $query
	 * @return XmlNodeInterface[]
	 */
	function xpath($query);

	/**
	 * @return XmlNodeInterface[]
	 */
	function getChildObjects();

	/**
	 * @return XmlNodeInterface
	 */
	function parent();

	/**
	 * @return bool
	 */
	function isXmlNode();
}
