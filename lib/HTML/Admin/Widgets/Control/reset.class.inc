<?php
class PXControlReset extends PXAdminWidget {
	var $value;

	public function __construct($value) {
		$this->value = $value;
	}
	function html() {
		return '<input type="reset" value="'.quot($this->value).'" '.$this->disable.'>';
	}
}
