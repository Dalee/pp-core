<?php
class NLGetData extends NLHTTPVarsData {
	function __construct($needToStripTags = false) {
		parent::__construct($needToStripTags);
		$this->raw = $_GET;
		$this->_NormalizeData();
	}

}?>
