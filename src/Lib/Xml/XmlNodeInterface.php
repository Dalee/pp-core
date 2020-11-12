<?php

namespace PP\Lib\Xml;

/**
 * Interface XmlNodeInterface
 * @package PP\Lib\Xml
 */
interface XmlNodeInterface
{

	/**
	 * @return string
	 */
	public function nodeName();

	/**
	 * @return string
	 */
	public function nodeValue();

	/**
	 * @param $xpath
	 * @return string
	 */
	public function nodeXValue($xpath);

	/**
	 * @return mixed
	 */
	public function nodeType();

	/**
	 * @return array
	 */
	public function attributes();

	/**
	 * @param string $attrName
	 * @return mixed
	 */
	public function getAttribute($attrName);

	/**
	 * @return XmlNodeInterface[]
	 */
	public function childNodes();

	/**
	 * @param string $query
	 * @return XmlNodeInterface[]
	 */
	public function xpath($query);

	/**
	 * @return XmlNodeInterface[]
	 */
	public function getChildObjects();

	/**
	 * @return XmlNodeInterface
	 */
	public function parent();

	/**
	 * @return bool
	 */
	public function isXmlNode();
}
