<?php
class PXInputTextarea extends PXAdminWidget {
	var $value;
	var $name;

	function PXInputTextarea($name, $value, $height, $wrap = "physical") {
		$this->value  = $value;
		$this->name   = $name;
		$this->height = $height;
		$this->wrap   = $wrap;
	}

	function html() {
		$height = strpos($this->height, '%') ? $this->height : $this->height.'px';

		return <<<HTML
			<textarea name="{$this->name}" wrap="{$this->wrap}" style="height: {$height};" {$this->disable}>{$this->value}</textarea>
HTML;
	}
}
?>