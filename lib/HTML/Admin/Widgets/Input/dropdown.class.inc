<?php
class PXInputDropDown extends PXAdminWidget {
	var $name;
	var $value;
	var $values = array();

	var $multiple = '';
	var $size     = '';

	protected $empty = array(null => 0);

	public function __construct($name, $value, $values) {
		$this->name = $name;
		$this->value = $value;
		$this->values = $values;
	}

	function setMultiple($multiple){
		$this->multiple = $multiple ? 'multiple="multiple"' : '';
	}

	function setSize($size){
		$this->size = is_numeric($size) ? 'size="'.$size.'"' : '';
	}

	function html() {
		$selected = max($this->empty, @array_flip((array)$this->value));
		$html     = squeeze(sprintf('<select name="%s" %s %s %s>', $this->name, $this->disable, $this->multiple, $this->size), ' ');
		$optgroup = false;

		foreach ($this->values as $k => $title) {
			// simple drop down selector
			if (!is_array($title)) {
				$html .= $this->htmlOption($k, $title, $selected);
				continue;
			}

			$optgroup = true;

			// grouped selector
			$html .= sprintf('<optgroup label="%s">', quot($k));

			foreach ($title as $j => $jtitle) {
				$html .= $this->htmlOption($j, $jtitle, $selected);
			}

			$html .= '</optgroup>';
		}

		foreach ($selected as $unknown => $k) {
			$optgroup && $html .= '<optgroup label="...">';

			$html .= $this->htmlOption($unknown, sprintf('[нет в списке - "%s"]', $unknown), $selected, true);

			$optgroup && $html .= '</optgroup>';
		}

		$html .= '</select>';

		return $html;
	}

	protected function htmlOption($value, $title, &$selected, $isSelected = false) {
		$k = $value;

		// if there is no empty value and no "bydefault" value, try to use value == 0
		if (!$isSelected && (isset($selected[$k]) || ($selected == $this->empty && empty($k) && (($k = null) || true)))) {
			$isSelected = true;
			unset($selected[$k]);
		}

		return sprintf('<option value="%s"%s>%s</option>', quot($value), $isSelected ? ' selected' : '',
			htmlspecialchars($title, ENT_COMPAT|ENT_HTML401, DEFAULT_CHARSET)
		);
	}
}
