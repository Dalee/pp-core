<?php

/**
 * Class PXInputInput
 */
class PXInputInput extends PXAdminWidget {

	/**
	 * @var array
	 */
	public $classes = [];

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
	public function __construct(public $name, public $value, public ?int $maxlength = null)
 {
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
  * @return PXInputInput
  */
 public function setClasses(array $classes) {
		$this->classes = $classes;
		return $this;
	}

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
