<?php

namespace PP\Lib\UrlGenerator;

/**
 * Class ContextUrlGenerator
 * @package PP\Lib\UrlGenerator
 */
class ContextUrlGenerator {

	/** @var null|string */
	protected $targetAction;

	/** @var null|\PXRequest */
	protected $request;

	/** @var null|string */
	protected $targetModule;

	/** @var null|string */
	protected $currentModule;

	/**
	 * @return null|string
	 */
	public function getTargetAction() {
		return $this->targetAction;
	}

	/**
	 * @param string $targetAction
	 * @return $this
	 */
	public function setTargetAction($targetAction) {
		$this->targetAction = $targetAction;
		return $this;
	}

	/**
	 * @return bool
	 */
	public function hasRequest() {
		return isset($this->request);
	}

	/**
	 * @return null|\PXRequest
	 */
	public function getRequest() {
		return $this->request;
	}

	/**
	 * @param \PXRequest $request
	 * @return $this
	 */
	public function setRequest(\PXRequest $request) {
		$this->request = $request;
		return $this;
	}

	/**
	 * @return bool
	 */
	public function hasTargetModule() {
		return isset($this->targetModule);
	}

	/**
	 * @return null|string
	 */
	public function getTargetModule() {
		return $this->targetModule;
	}

	/**
	 * @param null|string $targetModule
	 * @return $this
	 */
	public function setTargetModule($targetModule) {
		$this->targetModule = $targetModule;
		return $this;
	}

	/**
	 * @return bool
	 */
	public function hasCurrentModule() {
		return isset($this->currentModule);
	}

	/**
	 * @return null|string
	 */
	public function getCurrentModule() {
		return $this->currentModule;
	}

	/**
	 * @param string $currentModule
	 * @return $this
	 */
	public function setCurrentModule($currentModule) {
		$this->currentModule = $currentModule;
		return $this;
	}

}
