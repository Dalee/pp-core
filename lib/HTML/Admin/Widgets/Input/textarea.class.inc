<?php
class PXInputTextarea extends PXAdminWidget {
	var $value;
	var $name;

	function PXInputTextarea($name, $value, $height) {
		$this->value  = $value;
		$this->name   = $name;
		$this->height = $height;
	}

	function html() {
		$height = strpos($this->height, '%') ? $this->height : $this->height.'px';

		return <<<HTML
			<textarea name="{$this->name}" wrap="physical" style="height: {$height};" {$this->disable}>{$this->value}</textarea>
HTML;
	}
}
?>