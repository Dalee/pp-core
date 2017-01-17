<?php

/**
 * Class PXInputInput
 */
class PXInputInput extends PXAdminWidget {

	/**
	 * @var string
	 */
	var $value;

	/**
	 * @var string
	 */
	var $name;

	/**
	 * @var int|null
	 */
	var $maxlength = null;

	/**
	 * @var string
	 */
	protected $placeholder = '';

	/**
	 * PXInputInput constructor.
	 *
	 * @param string $name
	 * @param string $value
	 * @param int|null $maxlength
	 */
	public function __construct($name, $value, $maxlength = null) {
		$this->value = $value;
		$this->name = $name;
		$this->maxlength = $maxlength;
	}

	/**
	 * @return string
	 */
	public function getPlaceholder() {
		return $this->placeholder;
	}

	/**
	 * @param string $placeholder
	 * @return $this
	 */
	public function setPlaceholder($placeholder) {
		$this->placeholder = $placeholder;
		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	public function html() {
		$value = htmlspecialchars($this->value, ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET);
		$maxlength = '';
		if (!is_null($this->maxlength)) {
			$maxlength = sprintf('maxlength="%d"', $this->maxlength);
		}

		return sprintf('<input type="text" placeholder="%s" value="%s" name="%s" %s %s />', $this->placeholder, $value, $this->name, $this->disable, $maxlength);
	}
}
