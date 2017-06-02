<?php
// abstract
class PXXSLT {
	var $xsl;
	var $parameters = array();

	public function __construct($xslFile) {
		$this->xsl = $xslFile;
	}

	function addParameter($key, $value) {
		$this->parameters[$key] = $value;
	}

	function setXSL($xsl) {
		$this->xsl = $xsl;
	}

	// abstract
	function transform($xml, $convertToKoi=true) {
		FatalError('It\s  abstract method');
	}
}
