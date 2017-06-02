<?php

class PXInputRadiobutton extends PXInputCheckbox {

	private $pattern = '<input type="radio" class="radio" name="%1$s" %2$s />';

	public function html() {
		$atts = $this->on;
		if ($this->id) {
			$atts .= ' id="' . $this->id . '"';
		}
		if (!is_null($this->value)) {
			$atts .= ' value="' . quot($this->value) . '"';
		}
		$atts .= $this->disable;

		return sprintf($this->pattern, quot($this->name), $atts);
	}
}
