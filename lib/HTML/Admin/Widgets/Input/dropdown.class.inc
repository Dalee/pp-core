<?php
class PXInputDropDown extends PXAdminWidget {
	var $name;
	var $value;
	var $values = array();
	
	function PXInputDropDown($name, $value, $values) {
		$this->name = $name;
		$this->value = $value;
		$this->values = $values;
	}

	function html() {
		$html = '<select name="'.$this->name.'"'.$this->disable.'>';

		$notSelected = true;
		foreach ($this->values as $k=>$v) {
			$selectedHTML = '';

			if($notSelected) {
				if($k == $this->value) {
					$selectedHTML = ' selected';
					$notSelected = false;
				}
			}

			$html .= '<option value="'.$k.'"'.$selectedHTML.'>'.$v.'</option>';
		}

		if(!in_array($this->value, array_keys($this->values))) {
			$html .= '<option value="'.$this->value.'" selected>[нет в списке - '.$this->value.']</option>';
		}

		$html .= '</select>';

		return $html;
	}
}
?>