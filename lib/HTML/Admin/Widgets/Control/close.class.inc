<?php
class PXControlClose extends PXAdminWidget {
	var $value;
	
	function PXControlClose($value) {
		$this->value = $value;
	}
	function html() {
		return '<button onclick="window.close();return false">'.$this->value.'</button>';
	}
}
	
?>