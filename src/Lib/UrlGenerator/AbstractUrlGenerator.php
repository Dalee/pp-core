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
	 * @param $context
	 */
	public function __construct(ContextUrlGenerator $context) {
		$this->context = $context;
	}

	/**
	 * @param array [string]string $params
	 * @return string
	 */
	public function generate($params = []) {
		$action = $this->context->getTargetAction();
		switch ($action) {
			case ModuleInterface::ACTION_INDEX:
				$url = $this->indexUrl($params);
				break;
			case ModuleInterface::ACTION_ACTION:
				$url = $this->actionUrl($params);
				break;
			case ModuleInterface::ACTION_JSON:
				$url = $this->jsonUrl($params);
				break;
			case ModuleInterface::ACTION_POPUP:
				$url = $this->popupUrl($params);
				break;
			default:
				throw new \LogicException("Action '$action' doesn't exist.");
		}
		return $url;
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

}
