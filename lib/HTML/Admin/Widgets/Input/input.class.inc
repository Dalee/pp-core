<?php
class PXInputInput extends PXAdminWidget {
	var $value;
	var $name;

	function PXInputInput($name, $value) {
		$this->value = $value;
		$this->name = $name;
	}

	function html() {
		return '<input type="text" name="'.$this->name.'" value="'.$this->value.'" '.$this->disable.'>';
	}
}

?>