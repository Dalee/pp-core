<?php
class PXInputCheckbox extends PXAdminWidget {
	var $name;
	var $on;
	var $value = NULL;
	var $id = NULL;

	function PXInputCheckbox($name, $on=false, $value=NULL) {
		$this->name = $name;
		$this->setOn($on);
		$this->setValue($value);
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
		$html .= !is_null($this->value) ? ' value="'.quot($this->value).'"' : '';
		$html .= $this->disable.' />';

		return $html;
	}
}
?>