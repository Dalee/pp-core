<?php

namespace PP\Lib\UrlGenerator;

use PP\Module\AbstractModule;
use PP\Module\ModuleInterface;

/**
 * Class ContextUrlGenerator
 * @package PP\Lib\UrlGenerator
 */
class ContextUrlGenerator {

	/** @var string */
	protected $targetAction;

	/** @var null|\PXRequest */
	protected $request;

	/** @var null|string */
	protected $targetModule;

	/** @var null|string */
	protected $currentModule;

	/**
	 * Context constructor.
	 * @param string $targetAction
	 * @param \PXRequest $request
	 * @param string $targetModule
	 */
	public function __construct(
		$targetAction = ModuleInterface::ACTION_INDEX,
		\PXRequest $request = null,
		$targetModule = null
	) {
		$this->request = $request;
		$this->targetModule = $targetModule;
		$this->targetAction = $targetAction;
	}

	/**
	 * @return string
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
	 * @param null|\PXRequest $request
	 * @return $this
	 */
	public function setRequest(\PXRequest$request) {
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
