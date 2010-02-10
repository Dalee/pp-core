<?php
class NLGetData extends NLHTTPVarsData {
	function __construct($needToStripTags = false) {
		parent::__construct($needToStripTags);
		$this->raw = (array) $_GET;
		$needFixIisRewrite = IS_WIN;
		$this->_NormalizeData($needFixIisRewrite);
	}
}
?>
