<?php
class PXInputColor extends PXAdminWidget {
	var $name;
	var $value;
	
	function PXInputColor($name, $value) {
		$this->name = $name;
		$this->value = $value;
	}

	function html() {
		$name = $this->name;

		$red   = hexdec(substr($this->value, 0, 2));
		$green = hexdec(substr($this->value, 2, 2));
		$blue  = hexdec(substr($this->value, 4, 2));

		return <<<HTML
			<div class="colorselect">
				<div id="colorBG{$name}" style="background-color: #{$this->value};"></div>

				<input type="text" name="{$name}" id="colorData{$name}" value="{$this->value}" {$this->disable} readonly>

				<input type="hidden" id="colorRed{$name}"    value="{$red}">
				<input type="hidden" id="colorGreen{$name}"  value="{$green}">
				<input type="hidden" id="colorBlue{$name}"   value="{$blue}">

				<button title="изменить цвет" onclick="with(this.form) ctrlColorer.DoModal(
					this.form.colorData{$name},
					this.form.colorRed{$name},
					this.form.colorGreen{$name},
					this.form.colorBlue{$name},
					document.getElementById('colorBG{$name}')
				);
				return false;" onFocus="this.blur();"></button>

				<iframe width="400" height="300" src="tools/colorer/index.html" name="color{$name}" id="color{$name}" scrolling="no" frameborder="0"></iframe>
			</div>
HTML;
	}
}
?>