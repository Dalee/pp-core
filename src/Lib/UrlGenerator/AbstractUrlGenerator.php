<?php

namespace PP\Lib\UrlGenerator;

/**
 * Class AbstractUrlGenerator
 * @package PP\Lib\UrlGenerator
 */
abstract class AbstractUrlGenerator implements GeneratorInterface {

	const ACTION_INDEX = 'index';
	const ACTION_ACTION = 'action';
	const ACTION_JSON = 'json';
	const ACTION_POPUP = 'popup';

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
			case static::ACTION_INDEX:
				$url = $this->indexUrl($params);
				break;
			case static::ACTION_ACTION:
				$url = $this->indexUrl($params);
				break;
			case static::ACTION_JSON:
				$url = $this->indexUrl($params);
				break;
			case static::ACTION_POPUP:
				$url = $this->indexUrl($params);
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
	 * @param ContextUrlGenerator $context
	 * @return AbstractUrlGenerator
	 */
	public function setContext($context) {
		$this->context = $context;
		return $this;
	}

	/**
	 * @return string
	 */
	protected function getArea() {
		if (!$this->context->hasCurrentModule() && !$this->context->hasTargetModule()) {
			throw new \LogicException('Don\'t given target module and current module.');
		}
		$area = $this->context->getTargetModule();
		if ($area === null) {
			$area = $this->context->getCurrentModule()->area;
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
