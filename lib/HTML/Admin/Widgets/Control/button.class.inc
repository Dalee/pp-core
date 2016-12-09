<?php

class PXControlButton extends PXAdminWidget {

	var $value;
	var $onClickCode = '';
	var $cssClass = '';

	public function __construct($value) {
		$this->value = $value;
	}

	/**
	 * @param string $onClickCode
	 * @return $this
	 */
	function setClickCode($onClickCode) {
		$this->onClickCode = $onClickCode;

		return $this;
	}

	/**
	 * @param string $cssClass
	 * @return $this
	 */
	function setClass($cssClass) {
		$this->cssClass = $cssClass;

		return $this;
	}

	/**
	 * @return string
	 */
	function html() {
		$cssClass = mb_strlen($this->cssClass) ? ' class="'.$this->cssClass.'"' : '';
		$onClick = mb_strlen($this->onClickCode) ? ' onclick="'.$this->onClickCode.'"' : '';

		return '<button'.$cssClass.$onClick.$this->disable.'>'.$this->value.'</button>';
	}
}
