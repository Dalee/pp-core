<?php

class PXmlDomxml extends PXmlAbstract implements PXmlInterface {
	function PXmlDomxml($xmlEntity){
		$this->xmlObject = $this->identEntity($xmlEntity, 'domelement', 'domxml_open_file', 'domxml_open_mem');
	}

	function xpath($query){
		$nodesContainer = array();

		if(is_object($this->xmlObject)){
			$dom   = xpath_new_context($this->xmlObject);
			$res   = xpath_eval($dom, $query);

			if($res){
				$nodes = $res->nodeset;

				foreach($nodes as $node){
					$nodesContainer[] = new PXmlDomxmlNode($node);
				}
			}
		}

		return $nodesContainer;
	}
}

class PXmlDomxmlNode extends PXmlAbstractNode {

	function PXmlDomxmlNode($node){
		parent::PXmlAbstractNode($node);
	}

	function nodeName(){
		if(isset($this->_nodeName)){
			return $this->_nodeName;
		}

		$this->_nodeName = $this->_node->node_name();

		return $this->_nodeName;
	}

	function nodeValue(){
		if(isset($this->_nodeValue)){
			return $this->_nodeValue;
		}

		$this->_nodeValue = $this->_node->get_content();

		return $this->_nodeValue;
	}

	function nodeType(){
		if(isset($this->_nodeType)){
			return $this->_nodeType;
		}

		$this->_nodeType = $this->_node->node_type();
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
					$this->_attributes[trim($v->name)] = $v;
				}
			}
		}

		return array_values($this->_attributes);
	}

	function &getChildObjects(){
		$xmlChilds = $this->_node->child_nodes();
		return $xmlChilds;
	}
}
?>
