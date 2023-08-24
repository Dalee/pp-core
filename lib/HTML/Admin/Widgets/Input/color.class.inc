<?php

class PXInputColor extends PXAdminWidget
{
    public function __construct(public $name, public $value)
    {
    }

    public function html()
    {
        $name = $this->name;
        $value = htmlspecialchars((string) $this->value, ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET);

        $red = hexdec(mb_substr((string) $this->value, 0, 2));
        $green = hexdec(mb_substr((string) $this->value, 2, 2));
        $blue = hexdec(mb_substr((string) $this->value, 4, 2));

        return <<<HTML
			<div class="colorselect">
				<div id="colorBG{$name}" style="background-color: #{$value};"></div>

				<input type="text" name="{$name}" id="colorData{$name}" value="{$value}" {$this->disable} readonly>

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
