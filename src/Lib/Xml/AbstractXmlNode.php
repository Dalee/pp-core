<?php

namespace PP\Lib\Xml;

/**
 * Class AbstractXmlNode
 * @package PP\Lib\Xml
 */
abstract class AbstractXmlNode implements XmlNodeInterface {
	/** @var  XmlNodeInterface */
	protected $_xml;
	protected $_node;

	protected $_nodeName;
	protected $_nodeValue;
	protected $_nodeType;
	protected $_attributes;
	protected $_childNodes;

	/**
	 * AbstractXmlNode constructor.
	 * @param $node
	 */
	public function __construct($node) {
		$this->_node = $node;
	}

	/**
	 * @return mixed
	 */
	abstract protected function _createXmlContext();

	/**
	 * {@inheritdoc}
	 */
	function xpath($query) {
		$this->_createXmlContext();
		return $this->_xml->xpath($query);
	}

	/**
	 * {@inheritdoc}
	 */
	function nodeName() {
	}

	/**
	 * {@inheritdoc}
	 */
	function nodeValue() {
	}

	/**
	 * {@inheritdoc}
	 */
	function nodeType() {
	}

	/**
	 * {@inheritdoc}
	 */
	function attributes() {
	}

	/**
	 * {@inheritdoc}
	 */
	function isXmlNode() {
		return $this->nodeType() == XML_ELEMENT_NODE;
	}

	/**
	 * {@inheritdoc}
	 */
	function childNodes() {
		if (isset($this->_childNodes)) {
			return $this->_childNodes;
		}

		$this->_childNodes = array();

		if (is_object($this->_node)) {
			$self = get_class($this);
			$childs = $this->getChildObjects();

			if ($childs) {
				foreach ($childs as $node) {
					$this->_childNodes[] = new $self($node);
				}
			}
		}

		return $this->_childNodes;
	}

	/**
	 * {@inheritdoc}
	 */
	function getChildObjects() {
		return array();
	}

	/**
	 * @param $nodeName
	 * @return array|mixed|null
	 */
	function getChildNode($nodeName) {
		$find = array();

		foreach ($this->childNodes() as $node) {
			if ($node->nodeName() == $nodeName) {
				$find[] = $node;
			}
		}

		if (!sizeof($find)) {
			return null;
		}

		return sizeof($find) == 1 ? $find[0] : $find;
	}

	/**
	 * @param $name
	 * @return array|bool|mixed|null
	 */
	function __get($name) {
		$attr = $this->getAttribute($name);

		if ($attr !== FALSE) {
			return $attr;
		}

		return $this->getChildNode($name);

	}

	/**
	 * {@inheritdoc}
	 */
	function getAttribute($attrName) {
		$this->attributes();

		if (count($this->_attributes) > 0 && isset($this->_attributes[$attrName])) {
			return $this->_attributes[$attrName]->value;
		}

		return false;
	}

	/**
	 * {@inheritdoc}
	 */
	function parent($selector = null) {
		$ret = $this->xpath('..'); //TODO: make here selector context apply
		return reset($ret);
	}
}
