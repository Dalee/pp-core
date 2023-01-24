<?php

namespace PP\Lib\Xml;

/**
 * Class AbstractXmlNode
 * @package PP\Lib\Xml
 */
abstract class AbstractXmlNode implements XmlNodeInterface {
	/** @var  XmlNodeInterface */
	protected $_xml;

	protected $_nodeName;
	protected $_nodeValue;
	protected $_nodeType;
	protected $_attributes;
	protected $_childNodes;

	/**
	 * AbstractXmlNode constructor.
	 * @param $node
	 */
	public function __construct(protected $_node)
	{
	}

	/**
	 * @return mixed
	 */
	abstract protected function _createXmlContext();

	/**
	 * {@inheritdoc}
	 */
	public function xpath($query) {
		$this->_createXmlContext();
		return $this->_xml->xpath($query);
	}

	/**
	 * {@inheritdoc}
	 */
	public function nodeName() {
	}

	/**
	 * {@inheritdoc}
	 */
	public function nodeValue() {
	}

	/**
	 * {@inheritdoc}
	 */
	public function nodeType() {
	}

	/**
	 * {@inheritdoc}
	 */
	public function attributes() {
	}

	/**
	 * {@inheritdoc}
	 */
	public function isXmlNode() {
		return $this->nodeType() == XML_ELEMENT_NODE;
	}

	/**
	 * {@inheritdoc}
	 */
	public function childNodes() {
		if (isset($this->_childNodes)) {
			return $this->_childNodes;
		}

		$this->_childNodes = [];

		if (is_object($this->_node)) {
			$self = static::class;
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
	public function getChildObjects() {
		return [];
	}

	/**
	 * @param $nodeName
	 * @return array|mixed|null
	 */
	public function getChildNode($nodeName) {
		$find = [];

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
	public function __get($name) {
		$attr = $this->getAttribute($name);

		if ($attr !== false) {
			return $attr;
		}

		return $this->getChildNode($name);

	}

	/**
	 * {@inheritdoc}
	 */
	public function getAttribute($attrName) {
		$this->attributes();

		if ((is_countable($this->_attributes) ? count($this->_attributes) : 0) > 0 && isset($this->_attributes[$attrName])) {
			return $this->_attributes[$attrName]->value;
		}

		return false;
	}

	/**
	 * {@inheritdoc}
	 */
	public function parent($selector = null) {
		$ret = $this->xpath('..'); //TODO: make here selector context apply
		return reset($ret);
	}
}
