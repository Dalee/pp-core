<?php

namespace PP\Lib\Xml;

/**
 * Class SimpleXmlNode
 * @package PP\Lib\Xml
 */
class SimpleXmlNode extends AbstractXmlNode {

	/**
	 * {@inheritdoc}
	 */
	protected function _createXmlContext() {
		if ($this->_xml instanceof AbstractXml) {
			return;
		}

		// dat is strange...
		$this->_xml = new SimpleXml($this->_node);
	}

	/**
	 * {@inheritdoc}
	 */
	function nodeName() {
		if (isset($this->_nodeName)) {
			return $this->_nodeName;
		}

		$this->_nodeName = $this->_node->getName();
		return $this->_nodeName;
	}

	/**
	 * {@inheritdoc}
	 */
	function nodeValue() {
		if (isset($this->_nodeValue)) {
			return $this->_nodeValue;
		}

		$this->_nodeValue = (string)$this->_node;
		return $this->_nodeValue;
	}

	/**
	 * {@inheritdoc}
	 */
	function nodeType() {
		if (isset($this->_nodeType)) {
			return $this->_nodeType;
		}

		$n = $this->_node;
		switch (true) {
			case $n->xpath('/*') == array($n);
				$this->_nodeType = Xml::DOC;
				break;
			case ($n->xpath('.') == array($n)):
				$this->_nodeType = Xml::ELEMENT;
				break;
			case $n->attributes() === null:
			case $n[0] == $n:
				$this->_nodeType = Xml::ATTRIBUTE;
				break;
			default:
				$this->_nodeType = Xml::NONE;
				break;
		}
		return $this->_nodeType;
	}

	/**
	 * {@inheritdoc}
	 */
	function nodeXValue($xpath) {
		list ($node) = (array)($this->_node->xpath($xpath)) + array(0 => '');
		return (string)$node;
	}

	/**
	 * {@inheritdoc}
	 */
	function attributes() {
		if (isset($this->_attributes)) {
			return array_values($this->_attributes);
		}

		$this->_attributes = array();

		if (is_object($this->_node)) {
			$attrs = $this->_node->attributes();

			if ($attrs) {
				foreach ($attrs as $k => $v) {
					$this->_attributes[trim($k)] = Xml::attributePrototype($k, (string)$v);
				}
			}
		}

		return array_values($this->_attributes);
	}

	/**
	 * {@inheritdoc}
	 */
	function getChildObjects() {
		$xmlChilds = $this->_node->children();
		return $xmlChilds;
	}
}
