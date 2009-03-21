<?php
class PXControlButton extends PXAdminWidget {
	var $value;
	var $onClickCode;

	function PXControlButton($value) {
		$this->value = $value;
	}

	function setClickCode($onClickCode) {
		$this->onClickCode = $onClickCode;
	}

	function html() {
		return '<button '.($this->onClickCode ? 'onclick="'.$this->onClickCode.'"' : '').''.$this->disable.'>'.$this->value.'</button>';
	}
}
?>