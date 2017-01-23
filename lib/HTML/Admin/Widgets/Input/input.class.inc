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
	 * @var array
	 */
	var $classes = [];

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
	 * @return array
	 */
	public function getClasses() {
		return $this->classes;
	}

	/**
	 * @param array $classes
	 * @return PXInputInput
	 */
	public function setClasses(array $classes) {
		$this->classes = $classes;
		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	public function html() {
		$value = htmlspecialchars($this->value, ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET);
		$maxlength = '';
		if (!is_null($this->maxlength)) {
			$maxlength = 'maxlength=' . $this->maxlength;
		}

		return sprintf(
			'<input type="text" class="%s" placeholder="%s" value="%s" name="%s" %s %s />',
			join(' ', $this->classes), $this->placeholder, $value, $this->name, $this->disable, $maxlength
		);
	}
}
