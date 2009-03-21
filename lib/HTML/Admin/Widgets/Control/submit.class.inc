<?php
class PXControlSubmit extends PXAdminWidget {
	var $value;
	var $name = NULL;

	function PXControlSubmit($value, $name) {
		$this->value = $value;
		$this->name = $name;
	}

	function html() {
		return '<input type="submit"'.(!empty($this->name) ? ' name="'.$this->name.'"' : '').' value="'.$this->value.'" accesskey="s"'.$this->disable.'>';
	}
}
?>