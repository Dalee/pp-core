<?php
class PXInputCheckbox extends PXAdminWidget {
	var $name;
	var $on;
	var $value = NULL;
	var $id = NULL;

	function PXInputCheckbox($name) {
		$this->name = $name;
	}

	function setOn($on) {
		 $this->on = $on ? 'checked' : '';
	}

	function setId($id) {
		$this->id = $id;
	}

	function setValue($value) {
		$this->value = $value;
	}

	function html() {
		$html = '<input type="checkbox" class="checkbox" name="'.$this->name.'" '.$this->on;

		$html .= $this->id    ? ' id="'.$this->id.'"'       : '';
		$html .= $this->value ? ' value="'.$this->value.'"' : '';
		$html .= $this->disable.' />';	

		return $html;
	}
}
?>