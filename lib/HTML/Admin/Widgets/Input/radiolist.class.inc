<?php

class PXInputRadiolist extends PXAdminWidget {

	private $name = null;
	private $selected = null;
	private $list = null;

	private $pattern = '<label for="%2$s">%1$s %3$s</label>';

	public function __construct($name, $selected, $list) {
		$this->name = $name;
		$this->selected = (string)$selected;
		$this->list = $list;
	}

	public function html() {
		if (empty($this->name) || empty($this->list)) {
			return '';
		}

		$out = '';
		foreach ($this->list as $value => $title) {
			// hm... I don't like it
			$id = str_replace(array('[', ']'), array('_', ''), $this->name) . '_' . preg_replace('/[^a-z0-9\-]/i', '', $value);
			$input = new PXInputRadioButton($this->name, $this->selected === (string)$value, $value);
			$input->setId($id);
			$out .= sprintf($this->pattern, $input->html(), quot($id), quot($title));
		}

		return $out;
	}
}
