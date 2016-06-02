<?php

use \PP\Lib\Html\Layout\LayoutAbstract;

class PXAdminList extends PXAdminWidget {
	var	$_list ;
	var	$_varName;
	var	$_selected;
	var	$_getData;

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

	function _buildHref($key, $val = NULL) {
		return LayoutAbstract::buildHref($key, $val, $this->_getData);
	}

}

