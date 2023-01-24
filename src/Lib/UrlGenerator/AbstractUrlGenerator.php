<?php

namespace PP\Lib\UrlGenerator;

use PP\Module\ModuleInterface;

/**
 * Class AbstractUrlGenerator
 * @package PP\Lib\UrlGenerator
 */
abstract class AbstractUrlGenerator implements GeneratorInterface {

	/** @var ContextUrlGenerator  */
	protected $context;

	/**
  * AbstractUrlGenerator constructor.
  */
 public function __construct(ContextUrlGenerator $context) {
		$this->context = $context;
	}

	/**
	 * {@inheritDoc}
	 */
	public function generate($params = []) {
		$action = $this->context->getTargetAction();
		return match ($action) {
			ModuleInterface::ACTION_INDEX => $this->indexUrl($params),
			ModuleInterface::ACTION_ACTION => $this->actionUrl($params),
			ModuleInterface::ACTION_JSON => $this->jsonUrl($params),
			ModuleInterface::ACTION_POPUP => $this->popupUrl($params),
			default => throw new \LogicException("Action '$action' doesn't exist."),
		};
	}

	/**
	 * @return ContextUrlGenerator
	 */
	public function getContext() {
		return $this->context;
	}

	/**
	 * @return string
	 * @throws \LogicException
	 */
	protected function getArea() {
		if (!$this->context->hasCurrentModule() && !$this->context->hasTargetModule()) {
			throw new \LogicException('Don\'t given target module and current module.');
		}
		$area = $this->context->getTargetModule();
		if ($area === null) {
			$area = $this->context->getCurrentModule();
		}
		return $area;
	}

	/**
	 * @param string $url
	 * @param array [string]string $params
	 * @return string
	 */
	protected function generateUrl($url, $params = []) {
		$queryString = http_build_query($params);
		return "$url?$queryString";
	}

	/**
	 * @return string|null
	 */
	protected function getSid() {
		$sid = null;
		if ($this->context->hasRequest()) {
			$sid = $this->context->getRequest()->getSid();
		}
		return $sid;
	}
}
