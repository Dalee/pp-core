<?php

namespace PP\Lib\UrlGenerator\Roles;

abstract class AbstractUrlGenerator implements GeneratorInterface {

	/** @var \PP\Lib\UrlGenerator\Context  */
	protected $context;

	/**
	 * AbstractUrlGenerator constructor.
	 * @param $context
	 */
	public function __construct(\PP\Lib\UrlGenerator\Context $context) {
		$this->context = $context;
	}

	/**
	 * @return \PP\Lib\UrlGenerator\Context
	 */
	public function getContext() {
		return $this->context;
	}

	/**
	 * @param \PP\Lib\UrlGenerator\Context $context
	 * @return AbstractUrlGenerator
	 */
	public function setContext($context) {
		$this->context = $context;
		return $this;
	}


}
