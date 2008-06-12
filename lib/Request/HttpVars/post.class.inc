<?php
class NLPostData extends NLHTTPVarsData {
	function __construct($needToStripTags = false) {
		parent::__construct($needToStripTags);
		$this->raw = $_POST;
		$this->_NormalizeData();
	}

	function _GetImageVar($varName, $isArray=FALSE) {
		$retArray = array();

		if (!empty($_FILES[$varName]['name'])) {
			$retArray = array_merge($retArray, $_FILES[$varName]);

			$this->_Normalize($retArray);

			if ($isArray) {
				$tmpArray = $retArray['name'];
				$retArray['name'] = array();
				foreach ($tmpArray as $k=>$v) {
					$retArray['name'][$k] = stripslashes($v);
				}
			} else {
				$retArray['name'] = stripslashes($retArray['name']);
			}
		}
		if (isset($this->raw[$varName.'_remove']) && is_array($this->raw[$varName.'_remove'])) {
			$retArray = array_merge($retArray, array('remove' => array_keys($this->raw[$varName.'_remove'])));
		}
		return $retArray;
	}

	function _GetFileVar($varName, $isArray=FALSE) {
		return $this->_GetImageVar($varName, $isArray);
	}
}
?>
