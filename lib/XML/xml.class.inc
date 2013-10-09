<?php
class PXml {
	const NONE = 0;
	const ELEMENT = 1;
	const ATTRIBUTE = 2;
	const DOC = 9;

	public $xml;

	function PXml($xmlEntity){
		switch(true){
			case extension_loaded("domxml"):
				$this->xml = new PXmlDomxml($xmlEntity);
				break;
			case extension_loaded("simplexml"):
				$this->xml = new PXmlSimplexml($xmlEntity);
				break;
			default:
				$this->xml = (object)array('xmlObject' => false);
		}
	}

	public static function load($fileName){
		$instance = new PXml($fileName);
		return $instance->xml->xmlObject ? $instance->xml : false;
	}

	public static function loadString($xmlDataInString){
		return PXml::load($xmlDataInString);
	}

	// XML data unified prototypes
	
	public static function attributePrototype($name, $value){
		return (object)array('name' => $name, 'value' => $value);
	}
}
?>
