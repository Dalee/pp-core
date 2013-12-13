<?php
class PXInputInput extends PXAdminWidget {
	var $value;
	var $name;
	var $maxlength = null;

	function PXInputInput($name, $value, $maxlength=null) {
		$this->value = $value;
		$this->name = $name;
		$this->maxlength = $maxlength;
	}

	function html() {
		return '<input type="text" name="'.$this->name.'" value="'. htmlspecialchars($this->value, ENT_COMPAT|ENT_HTML401, DEFAULT_CHARSET) .'" '.$this->disable.( !is_null($this->maxlength) ? ' maxlength="'.$this->maxlength.'" ' : '').'>';
	}
}

?>
