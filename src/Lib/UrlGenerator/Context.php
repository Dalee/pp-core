<?php

namespace PP\Lib\UrlGenerator;


class Context {

	/** @var \PXRequest */
	protected $request;

	/** @var string */
	protected $module;

	/**
	 * Context constructor.
	 * @param string $module
	 * @param null $request
	 */
	public function __construct($request = null, $module = null) {
		$this->request = $request;
		$this->module = $module;
	}

	/**
	 * @return \PXRequest
	 */
	public function getRequest() {
		return $this->request;
	}

	/**
	 * @param \PXRequest $request
	 * @return Context
	 */
	public function setRequest($request) {
		$this->request = $request;
		return $this;
	}

	/**
	 * @return string
	 */
	public function getModule() {
		return $this->module;
	}

	/**
	 * @param string $module
	 * @return Context
	 */
	public function setModule($module) {
		$this->module = $module;
		return $this;
	}


}
