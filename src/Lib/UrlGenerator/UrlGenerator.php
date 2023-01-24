<?php

namespace PP\Lib\UrlGenerator;

/**
 * Class UrlGenerator
 * @package PP\Lib\UrlGenerator
 */
class UrlGenerator {

	/** @var ContextUrlGenerator */
	protected $context;

	/** @var AdminUrlGenerator */
	protected $adminGeneratorInstance;

	/** @var UserUrlGenerator */
	protected $userGeneratorInstance;

	/**
	* UrlGenerator constructor.
	*/
	public function __construct(ContextUrlGenerator $context) {
		$this->context = $context;
	}

	/**
	 * @return GeneratorInterface
	 */
	public function getUserGenerator() {
		if ($this->userGeneratorInstance === null) {
			$this->userGeneratorInstance = new UserUrlGenerator($this->context);
		}
		return $this->userGeneratorInstance;
	}

	/**
	 * @return GeneratorInterface
	 */
	public function getAdminGenerator() {
		if ($this->adminGeneratorInstance === null) {
			$this->adminGeneratorInstance = new AdminUrlGenerator($this->context);
		}
		return $this->adminGeneratorInstance;
	}

	/**
	 * @return ContextUrlGenerator
	 */
	public function getContext() {
		return $this->context;
	}

	/**
	 * @param ContextUrlGenerator $context
	 * @return $this
	 */
	public function setContext($context) {
		if ($this->context !== $context) {
			$this->adminGeneratorInstance = null;
			$this->userGeneratorInstance = null;
		}
		$this->context = $context;
		return $this;
	}

}
