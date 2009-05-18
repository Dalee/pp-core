<?php
interface PXmlInterface {
	public function xpath($query);
}

abstract class PXmlAbstract implements PXmlInterface {
	public $xmlObject;
	
	function PXmlAbstract($xmlEntity){
		FatalError('Abstract XML object cannot be created!');
	}

	function identEntity($testingObject, $nodeType, $fileLoader, $stringLoader){
		switch(true){
			case is_a($testingObject, $nodeType):
				return $testingObject;
			case is_callable($fileLoader) && (file_exists($testingObject) || strlen(getFromArray(parse_url($testingObject), 'scheme'))):
				return @$fileLoader($testingObject);
			case is_callable($stringLoader) && is_string($testingObject):
				return @$stringLoader($testingObject);
			default:
				return false;
		}
	}
	
	function xpath($query){}
}

abstract class PXmlAbstractNode {
	protected $_xml;
	protected $_node;

	protected $_nodeName;
	protected $_nodeValue;
	protected $_nodeType;
	protected $_attributes;
	protected $_childNodes;

	function PXmlAbstractNode($node){
		$this->_node = $node;
	}

	function _createXmlContext(){
		if(isset($this->_xml) && is_a($this->_xml, 'PXmlAbstract')){
			return;
		}
		$klass      = substr(get_class($this), 0, -4);
		$this->_xml = new $klass($this->_node);
	}
	
	function xpath($query){
		$this->_createXmlContext();
		return $this->_xml->xpath($query);
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
				$this->_childNodes[] = new $self($node);
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
