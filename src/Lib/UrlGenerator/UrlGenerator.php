<?php

namespace PP\Lib\UrlGenerator;

/**
 * Class UrlGenerator
 * @package PP\Lib\UrlGenerator
 */
class UrlGenerator {

	/** @var ContextUrlGenerator */
	protected $context;

	/**
	 * UrlGenerator constructor.
	 * @param ContextUrlGenerator $context
	 */
	public function __construct(ContextUrlGenerator $context) {
		$this->context = $context;
	}

	/**
	 * @return UserUrlGenerator
	 */
	public function getUserGenerator() {
		return new UserUrlGenerator($this->context);
	}

	/**
	 * @return AdminUrlGenerator
	 */
	public function getAdminGenerator() {
		return new AdminUrlGenerator($this->context);
	}

	/**
	 * @return ContextUrlGenerator
	 */
	public function getContext() {
		return $this->context;
	}

	/**
	 * @param ContextUrlGenerator $context
	 * @return UrlGenerator
	 */
	public function setContext($context) {
		$this->context = $context;
		return $this;
	}

}
