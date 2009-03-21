<?php
class PXInputParentDropDown extends PXAdminWidget {
	var $name;
	var $value;
	var $values = array();

	function PXInputParentDropDown($name, $value, $values) {
		$this->name = $name;
		$this->value = $value;
		$this->values = $values;	

	}

	function html() {
		$html = '<select name="'.$this->name.'"'.$this->disable.'>';

		foreach ($this->values as $k=>$v) {
			if(isset($v['disabled'])){
				$html .= '<optgroup class="disabled" label="'.$v['title'].'"></optgroup>';
			} else {
				$html .= '<option value="'.$k.'"'.($k == $this->value ? ' selected' : '').'>'.$v['title'].'</option>';
			}
		}

		$html .= '</select>';

		return $html;
	}
}
?>