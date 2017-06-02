<?php

class PXInputIPAddr extends PXAdminWidget {
	var $value;
	var $name;

	public function __construct($name, $value) {
		$this->value = $value;
		$this->name = $name;
	}

	function html() {
		$name = $this->name;
		$ip = $this->value;

		$delim = '<input type="text" class="delim" value="." readonly tabindex="-1">';

		return <<<HTML
			<div class="ip">
				<input type="text" maxlength="3" name="{$name}[0]" value="{$ip[0]}" class="first">{$delim
		}<input type="text" maxlength="3" name="{$name}[1]" value="{$ip[1]}" class="middle">{$delim
		}<input type="text" maxlength="3" name="{$name}[2]" value="{$ip[2]}" class="middle">{$delim
		}<input type="text" maxlength="3" name="{$name}[3]" value="{$ip[3]}" class="last">
			</div>
HTML;
	}
}

