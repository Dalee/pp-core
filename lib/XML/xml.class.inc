<?php
class PXml {
	var $_engine;
	var $xml;

	function PXml($pathToXMLFile){
		$this->_chooseEngine();

		switch($this->_engine){
			case 'domxml':
				$this->xml = & new PXmlDomxml($pathToXMLFile);
				break;

			case 'simplexml':
				$this->xml = & new PXmlSimplexml($pathToXMLFile);
				break;

			default:
				$this->xml = (object)array('xmlObject' => false);
		}
	}
	
	function _chooseEngine(){
		if(function_exists('domxml_open_file')){
			$this->_engine = 'domxml';

		} elseif(function_exists('simplexml_load_file')) {
			$this->_engine = 'simplexml';

		} else {
			$this->_engine = false;
		}
	}

	function load($fileName){
		$instance = & new PXml($fileName);
		return $instance->xml->xmlObject ? $instance->xml : false;
	}
}

?>
