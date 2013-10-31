<?php
class PXControlButton extends PXAdminWidget {
	var $value;
	var $onClickCode = '';
	var $cssClass    = '';

	function PXControlButton($value) {
		$this->value = $value;
	}

	function setClickCode($onClickCode) {
		$this->onClickCode = $onClickCode;
	}

	function setClass($cssClass) {
		$this->cssClass = $cssClass;
	}

	function html() {
		$cssClass = mb_strlen($this->cssClass)    ? ' class="'.$this->cssClass.'"'      : '';
		$onClick  = mb_strlen($this->onClickCode) ? ' onclick="'.$this->onClickCode.'"' : '';

		return '<button'.$cssClass.$onClick.$this->disable.'>'.$this->value.'</button>';
	}
}
?>
