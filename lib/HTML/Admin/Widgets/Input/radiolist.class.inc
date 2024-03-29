<?php

class PXInputRadiolist extends PXAdminWidget {

	private ?string $selected = null;

	private string $pattern = '<label for="%2$s">%1$s %3$s</label>';

	public function __construct(private $name, $selected, private $list) {
		$this->selected = (string)$selected;
	}

	public function html() {
		if (empty($this->name) || empty($this->list)) {
			return '';
		}

		$out = '';
		foreach ($this->list as $value => $title) {
			// hm... I don't like it
			$id = str_replace(['[', ']'], ['_', ''], (string) $this->name) . '_' . preg_replace(
					'/[^a-z0-9\-]/i' . REGEX_MOD,
					'',
					(string) $value
				);
			$input = new PXInputRadioButton($this->name, $this->selected === (string)$value, $value);
			$input->setId($id);
			$out .= sprintf($this->pattern, $input->html(), quot($id), quot($title));
		}

		return $out;
	}
}
