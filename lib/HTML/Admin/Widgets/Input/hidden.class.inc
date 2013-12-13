<?php
class PXInputHidden extends PXAdminWidget {
	var $value;
	var $name;

	function PXInputHidden($name, $value) {
		$this->value = $value;
		$this->name  = $name;
	}

	function html() {
		$name = htmlspecialchars($this->name, ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET);
		$value = htmlspecialchars($this->value, ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET);

		return '<input type="hidden" name="' . $name . '" value="' . $value . '" ' . $this->disable . '>';
	}
}
