<?php
class PXInputTextarea extends PXAdminWidget {
	public $value;
	public $name;
	public $height;
	public $maxlength;
	public $wrap;

	function PXInputTextarea($name, $value, $height, $maxlength = null, $wrap = "physical") {
		$this->value     = $value;
		$this->name      = $name;
		$this->height    = $height;
		$this->maxlength = $maxlength; //FIXME: What is it mean ? - "cell_maxlength определяет максимальное число символов, выводимых в ячейке"
		$this->wrap      = $wrap;
	}

	function html() {
		$height = strpos($this->height, '%') ? $this->height : $this->height.'px';

		return <<<HTML
			<textarea name="{$this->name}" wrap="{$this->wrap}" style="height: {$height};" {$this->disable}>{$this->value}</textarea>
HTML;
	}
}
?>