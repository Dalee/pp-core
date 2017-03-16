<?php

namespace PP\Lib\UrlGenerator;

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

	/**
	 * Context constructor.
	 * @param string $targetAction
	 * @param \PXRequest $request
	 * @param string $targetModule
	 */
	public function __construct(
		$targetAction = AbstractUrlGenerator::ACTION_INDEX,
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
	 * @return ContextUrlGenerator
	 */
	public function setTargetAction($targetAction) {
		$this->targetAction = $targetAction;
		return $this;
	}

	/**
	 * @return null|\PXRequest
	 */
	public function getRequest() {
		return $this->request;
	}

	/**
	 * @param null|\PXRequest $request
	 * @return ContextUrlGenerator
	 */
	public function setRequest($request) {
		$this->request = $request;
		return $this;
	}

	/**
	 * @return null|string
	 */
	public function getTargetModule() {
		return $this->targetModule;
	}

	/**
	 * @param null|string $targetModule
	 * @return ContextUrlGenerator
	 */
	public function setTargetModule($targetModule) {
		$this->targetModule = $targetModule;
		return $this;
	}

}
