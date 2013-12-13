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
		$selected = false;
		$html = '<select name="' . $this->name . '"' . $this->disable . '>';

		foreach ($this->values as $k => $v) {
			if(isset($v['disabled'])) {
				$html .= '<optgroup class="disabled" label="' . quot($v['title']) . '"></optgroup>';
			} else {
				$k == $this->value && $selected = true;
				$html .= '<option value="' . quot($k) . '"' . ($k == $this->value ? ' selected' : '') . '>' . htmlspecialchars($v['title'], ENT_COMPAT|ENT_HTML401, DEFAULT_CHARSET) . '</option>';
			}
		}

		if(!empty($this->value) && !$selected) {
			$html .= sprintf('<option value="%s" selected>[нет в списке - "%s"]</option>', quot($this->value), htmlspecialchars($this->value, ENT_COMPAT|ENT_HTML401, DEFAULT_CHARSET));
		}

		$html .= '</select>';

		return $html;
	}
}
?>