<?php

class PXControlButton extends PXAdminWidget
{

	public $onClickCode = '';
	public $cssClass = '';

	public function __construct(public $value)
 {
 }

	/**
	 * @param string $onClickCode
	 * @return $this
	 */
	public function setClickCode($onClickCode)
	{
		$this->onClickCode = $onClickCode;

		return $this;
	}

	/**
	 * @param string $cssClass
	 * @return $this
	 */
	public function setClass($cssClass)
	{
		$this->cssClass = $cssClass;

		return $this;
	}

	/**
	 * @return string
	 */
	public function html()
	{
		$cssClass = mb_strlen((string) $this->cssClass) ? ' class="' . $this->cssClass . '"' : '';
		$onClick = mb_strlen((string) $this->onClickCode) ? ' onclick="' . $this->onClickCode . '"' : '';

		return '<button' . $cssClass . $onClick . $this->disable . '>' . $this->value . '</button>';
	}
}
