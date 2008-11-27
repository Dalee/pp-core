<?php
class PXAdminList {
	var	$_list ;
	var	$_varName;
	var	$_selected;
	var $_getData; 
	
	function PXAdminList($list) {
		$this->_list     = $list;
		$this->_varName  = '';
		$this->_selected = '';
		$this->_getData  = array();
	}
	
	function setVarName($varName) {
		$this->_varName = $varName;
	}
	
	function setSelected($selected) {
		$this->_selected = $selected;
	}
	
	function setGetData($getData) {
		if (is_array($getData)) {
			$this->_getData = $getData;
		}
	}

	function html() {
		$html = '<ul class="list">';
		foreach ($this->_list as $k=>$v) {
			$html .= '<li';
			if ($k == $this->_selected) $html .= ' class="selected"';
			$html .= '><a href="'.$this->_BuildHref($this->_varName, $k).'">' . $v .'</a>';
			$html .= '</li>';
		}
		$html .= '</ul>';
		return $html;
	}
	
	function _BuildHref($key, $value) {
		$href = "?";
		foreach ($this->_getData as $k=>$v) {
			if (!empty($v) && $k != $key) {
				if (is_array($v)) {
					foreach ($v as $sk=>$sv) {
						$href .= $k.'[]='.urlencode($sv).'&';
					}
				} else {
					$href .= $k.'='.urlencode($v).'&';
				}
			}
		}
		$href .= $key.'='.$value.'&';
		$href = substr($href, 0, -1);
		return $href;
	}
	
}
?>