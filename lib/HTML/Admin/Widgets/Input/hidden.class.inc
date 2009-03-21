<?php
class PXInputHidden extends PXAdminWidget {
	var $value;
	var $name;

	function PXInputHidden($name, $value) {
		$this->value = $value;
		$this->name  = $name;
	}

	function html() {
		return '<input type="hidden" name="'.$this->name.'" value="'.$this->value.'" '.$this->disable.'>';
	}
}
?>