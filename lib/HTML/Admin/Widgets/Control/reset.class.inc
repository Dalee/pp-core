<?php
class PXControlReset extends PXAdminWidget {
	var $value;
	
	function PXControlReset($value) {
		$this->value = $value;
	}
	function html() {
		return '<input type="reset" value="'.quot($this->value).'" '.$this->disable.'>';
	}
}
	
?>