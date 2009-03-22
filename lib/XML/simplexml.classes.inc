<?php
class PXmlSimplexml extends PXmlInterface {
	function PXmlSimplexml($pathToXMLFile){
		$this->xmlObject = @simplexml_load_file($pathToXMLFile);
	}

	function xpath($query){
		$nodesContainer = array();

		if(is_object($this->xmlObject)){
			$nodes = $this->xmlObject->xpath($query);

			if($nodes){
				foreach($nodes as $node){
					$nodesContainer[] = & new PXmlSimplexmlNode($node);
				}
			}
		}

		return $nodesContainer;
	}
}

class PXmlSimplexmlNode extends PXmlAbstractNode {
	function PXmlSimplexmlNode($node){
		parent::PXmlAbstractNode($node);
		$this->_nodeType = 1;
	}

	function nodeName(){
		if(isset($this->_nodeName)){
			return $this->_nodeName;
		}

		$this->_nodeName = $this->_node->getName();
		return $this->_nodeName;
	}

	function nodeValue(){
		if(isset($this->_nodeValue)){
			return $this->_nodeValue;
		}

		$this->_nodeValue = (string)$this->_node;
		return $this->_nodeValue;
	}

	function nodeType(){
		return $this->_nodeType;
	}

	function attributes(){
		if(isset($this->_attributes)){
			return array_values($this->_attributes);
		}

		$this->_attributes = array();

		if(is_object($this->_node)){
			$attrs = $this->_node->attributes();

			if($attrs){
				foreach($attrs as $k=>$v){
					$this->_attributes[trim($k)] = (object)array('name' => $k, 'value' => (string)$v);
				}
			}
		}

		return array_values($this->_attributes);
	}

	function &getChildObjects(){
		$domxmlChilds = $this->_node->children();

		return $domxmlChilds;
	}
}

?>