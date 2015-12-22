<?php
class NLCookieData extends NLHTTPVarsData {
	function __construct($needToStripTags = false) {
		parent::__construct($needToStripTags, true);
		$this->raw = (array) $_COOKIE;
		$this->_NormalizeData();
	}

	function _GetVar($varName) {
		if (!isset($this->raw[$varName])) {
			return NULL;
		}

		$value = $this->raw[$varName];

		return $value;
	}
}
?>
