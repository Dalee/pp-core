<?php

namespace PP\Lib\Xml;

/**
 * Class SimpleXmlNode
 * @package PP\Lib\Xml
 */
class SimpleXmlNode extends AbstractXmlNode
{
    /**
     * {@inheritdoc}
     */
    protected function _createXmlContext()
    {
        if ($this->_xml instanceof AbstractXml) {
            return;
        }

        // dat is strange...
        $this->_xml = new SimpleXml($this->_node);
    }

    /**
     * {@inheritdoc}
     */
    public function nodeName()
    {
        if (isset($this->_nodeName)) {
            return $this->_nodeName;
        }

        $this->_nodeName = $this->_node->getName();
        return $this->_nodeName;
    }

    /**
     * {@inheritdoc}
     */
    public function nodeValue()
    {
        if (isset($this->_nodeValue)) {
            return $this->_nodeValue;
        }

        $this->_nodeValue = (string)$this->_node;
        return $this->_nodeValue;
    }

    /**
     * {@inheritdoc}
     */
    public function nodeType()
    {
        if (isset($this->_nodeType)) {
            return $this->_nodeType;
        }

        $n = $this->_node;
        $this->_nodeType = match (true) {
            $n->xpath('/*') == [$n] => Xml::DOC,
            $n->xpath('.') == [$n] => Xml::ELEMENT,
            $n->attributes() === null, $n[0] == $n => Xml::ATTRIBUTE,
            default => Xml::NONE,
        };
        return $this->_nodeType;
    }

    /**
     * {@inheritdoc}
     */
    public function nodeXValue($xpath)
    {
        [$node] = (array)($this->_node->xpath($xpath)) + [0 => ''];
        return (string)$node;
    }

    /**
     * {@inheritdoc}
     */
    public function attributes()
    {
        if (isset($this->_attributes)) {
            return array_values($this->_attributes);
        }

        $this->_attributes = [];

        if (is_object($this->_node)) {
            $attrs = $this->_node->attributes();

            if ($attrs) {
                foreach ($attrs as $k => $v) {
                    $this->_attributes[trim((string) $k)] = Xml::attributePrototype($k, (string)$v);
                }
            }
        }

        return array_values($this->_attributes);
    }

    /**
     * {@inheritdoc}
     */
    public function getChildObjects()
    {
        return $this->_node->children();
    }
}
