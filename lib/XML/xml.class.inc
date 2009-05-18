<?php
class PXml {
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
}
?>
