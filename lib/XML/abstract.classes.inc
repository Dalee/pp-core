<?php
interface IPXml {
	function xpath($query);
}

interface IPXmlNode {
	function nodeName();
	function nodeValue();
	function nodeType();
	function attributes();
	function getAttribute($attrName);
	function childNodes();
	function xpath($query);
	function getChildObjects();
}

abstract class PXmlAbstract implements IPXml {
	public $xmlObject;

	function PXmlAbstract($xmlEntity){
		FatalError('Abstract XML object cannot be created!');
	}

	function identEntity($testingObject, $nodeType, $fileLoader, $stringLoader){
/*		$oldTrackErrors = ini_set('track_errors', 1);
		$oldError = error_reporting(0);
*/
		switch(true){
			case is_a($testingObject, $nodeType):
				$xmlObject = $testingObject;

			case is_callable($fileLoader) && (file_exists($testingObject) || strlen(getFromArray(parse_url($testingObject), 'scheme'))):
				$xmlObject = @$fileLoader($testingObject);

			case is_callable($stringLoader) && is_string($testingObject):
				$xmlObject = @$stringLoader($testingObject);

			default:
				$xmlObject = false;
		}
/*
		error_reporting($oldError);
		ini_set('track_errors', $oldTrackErrors);

		if(!is_object($xmlObject)) {
			d2($php_errormsg);
		}
*/
		return $xmlObject; 
	}
	
	function xpath($query){}
}

abstract class PXmlAbstractNode implements IPXmlNode {
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
			$childs = $this->getChildObjects();
			
			
			foreach($childs as $node){
				$this->_childNodes[] = new $self($node);
			}
		}

		return $this->_childNodes;
	}

	function getChildObjects(){
		return array();
	}

	function getAttribute($attrName){
		$this->attributes();

		if(count($this->_attributes) > 0 && isset($this->_attributes[$attrName])){
			return $this->_attributes[$attrName]->value;
		}

		return false;
	}
}
?>
