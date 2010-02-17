<?php
class NLPostData extends NLHTTPVarsData {
	function __construct($needToStripTags = false) {
		parent::__construct($needToStripTags);
		$this->raw = (array) $_POST;
		$this->_NormalizeData();
	}

	function setImageArray(&$retArray) {
		$tmpArray = $retArray['name'];
		$retArray['name'] = array();
		
		foreach ($tmpArray as $k=>$v) {
			$retArray['name'][$k] = stripslashes($v);
			
			if($retArray['error'][$k] != 0 || $retArray['size'][$k] == 0) {
				unset($retArray['name'][$k]);
			}
		}
	}

	function _GetImageVar($varName, $isArray=FALSE) {
		$retArray = array();

		if (!empty($_FILES[$varName]['name'])) {
			$retArray = $_FILES[$varName];

			$this->_Normalize($retArray);

			if ($isArray) {
				$this->setImageArray($retArray);
			} else {
				$retArray['name'] = stripslashes($retArray['name']);
				
				if($retArray['error'] != 0 || $retArray['size'] == 0) {
					$retArray = array();
				}
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
