<?php
class PXInputTextarea extends PXAdminWidget {
	public $value;
	public $name;
	public $height;
	public $maxlength;
	public $wrap;

	function PXInputTextarea($name, $value, $height, $maxlength = null, $wrap = "soft") {
		$this->value     = $value;
		$this->name      = $name;
		$this->height    = $height;
		$this->maxlength = $maxlength; //Only Chrome and FF support this attribute
		$this->wrap      = $wrap;
	}

	function html() {
		$height = strpos($this->height, '%') ? $this->height : $this->height.'px';
		$max    = isset($this->maxlength) ? 'maxlength="' . (int)$this->maxlength . '"': '';

		return sprintf('<textarea name="%s" wrap="%s" style="height: %s;" %s %s>%s</textarea>',
		               $this->name, $this->wrap, $height, $this->disable, $max, htmlspecialchars($this->value, ENT_COMPAT|ENT_HTML401|ENT_QUOTES, DEFAULT_CHARSET));
	}
}
