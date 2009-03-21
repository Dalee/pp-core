<?php
class PXmlInterface {
	var $xmlObject;

	function xpath($query){}
}

abstract class PXmlAbstractNode {
	var $_node;

	var $_nodeName;
	var $_nodeValue;
	var $_nodeType;
	var $_attributes;
	var $_childNodes;

	function PXmlAbstractNode($node){
		$this->_node = $node;
	}

	function nodeName(){}
	function nodeValue(){}
	function nodeType(){}
	function attributes(){}

	function childNodes(){
		if(isset($this->_childNodes)){
			return $this->_childNodes;
		}

		$this->_childNodes = array();

		if(is_object($this->_node)){
			$self   = get_class($this);
			$childs = & $this->getChildObjects();

			foreach($childs as $node){
				$this->_childNodes[] = & new $self($node);
			}
		}

		return $this->_childNodes;
	}

	function &getChildObjects(){}

	function getAttribute($attrName){
		$this->attributes();

		if(count($this->_attributes) > 0 && isset($this->_attributes[$attrName])){
			return $this->_attributes[$attrName]->value;
		}

		return false;
	}
}
?>
