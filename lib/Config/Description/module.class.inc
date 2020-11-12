<?php

use PP\Module\ModuleInterface;

/**
 * Class PXModuleDescription.
 *
 * Holds module configuration from modules.yml
 */
class PXModuleDescription {

	/**
	 * @var string
	 */
	public const EMPTY_DESCRIPTION = '---';

	/**
	 * @var string
	 */
	protected $name;

	/**
	 * @var string
	 */
	protected $description;

	/**
	 * @var string
	 */
	protected $class;

	/**
	 * used by local/cronruns/subscribenews.cronrun.inc:105
	 * @var array
	 */
	public $settings = [];

	/**
	 * @return string
	 */
	public function getName() {
		return $this->name;
	}

	/**
	 * @param string $name
	 * @return $this
	 */
	public function setName($name) {
		$this->name = $name;

		return $this;
	}

	/**
	 * @return string
	 */
	public function getDescription() {
		return $this->description;
	}

	/**
	 * @param string $description
	 * @return $this
	 */
	public function setDescription($description) {
		$this->description = $description;

		return $this;
	}

	/**
	 * @return string
	 */
	public function getClass() {
		return $this->class;
	}

	/**
	 * @param string $class
	 * @return $this
	 */
	public function setClass($class) {
		$this->class = $class;

		return $this;
	}

	/**
	 * @return array
	 */
	public function getSettings() {
		return $this->settings;
	}

	/**
	 * @param $setting
	 * @return string|null
	 */
	public function getSetting($setting) {
		return isset($this->settings[$setting])? $this->settings[$setting]: null;
	}

	/**
	 * @param array $settings
	 * @return $this
	 */
	public function setSettings($settings) {
		$this->settings = $settings;

		return $this;
	}

	/**
	 * @return ModuleInterface
	 */
	public function getModule() {
		return new $this->class($this->name, $this->settings, $this);
	}

}

