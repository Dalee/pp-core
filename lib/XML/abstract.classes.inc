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

class PXmlErrors {
	public $errors = array();

	private function __contructor() {
	}

	public static function getErrors() {
		$instance = self::getInstance();
		return $instance->errors;
	}

	public static function addError($errno, $errstr, $errfile, $errline, $errcontext) {
		$instance = self::getInstance();
		$instance->errors[] = $errstr;
	}

	public static function getInstance() {
		static $instance;

		if(!is_object($instance)) {
			$instance = new self();
		}

		return $instance;
	}
}

abstract class PXmlAbstract implements IPXml {
	public $xmlObject;
	public $errors;

	function PXmlAbstract($xmlEntity){
		FatalError('Abstract XML object cannot be created!');
	}

	function identEntity($testingObject, $nodeType, $fileLoader, $stringLoader){
		set_error_handler(array('PXmlErrors', 'addError'), E_ALL);

		switch(true){
			case is_a($testingObject, $nodeType):
				$xmlObject = $testingObject;
				break;

			case is_callable($fileLoader) && (file_exists($testingObject) || strlen(getFromArray(parse_url($testingObject), 'scheme'))):
				$xmlObject = @$fileLoader($testingObject);
				break;

			case is_callable($stringLoader) && is_string($testingObject):
				$xmlObject = @$stringLoader($testingObject);
				break;

			default:
				$xmlObject = false;
				break;
		}

		restore_error_handler();

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

	function getChildNode($nodeName) {
		$find = array();

		foreach($this->childNodes() as $node) {
			if($node->nodeName() == $nodeName) {
				$find[] = $node;
			}
		}

		if(!sizeof($find)) {
			return null;
		}

		return sizeof($find) == 1 ? $find[0] : $find;
	}

	function __get($name) {
		$attr = $this->getAttribute($name);

		if($attr !== FALSE) {
			return $attr;
		}

		return $this->getChildNode($name);

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
