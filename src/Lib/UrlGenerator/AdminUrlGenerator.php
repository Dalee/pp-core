<?php

namespace PP\Lib\UrlGenerator;

class AdminUrlGenerator extends AbstractUrlGenerator {

	/**
	 * @param array [string]string $params
	 * @return string
	 */
	public function indexUrl($params = []) {
		if (!$this->context->hasCurrentModule() && !$this->context->hasTargetModule()) {
			throw new \LogicException('Don\'t given target module and current module.');
		}
		$url = '/admin/?';
		$targetModule = $this->context->getTargetModule();
		if ($targetModule === null) {
			$targetModule = $this->context->getCurrentModule()->area;
		}
		$params['area'] = $targetModule;
		$queryString = http_build_query($params);
		$url .= $queryString;
		return $url;
	}

	/**
	 * @param array [string]string $params
	 * @return string
	 */
	public function actionUrl($params = []) {
		// TODO: Implement actionUrl() method.
	}

	/**
	 * @param array [string]string $params
	 * @return string
	 */
	public function jsonUrl($params = []) {
		// TODO: Implement jsonUrl() method.
	}

	/**
	 * @param array [string]string $params
	 * @return string
	 */
	public function popupUrl($params = []) {
		// TODO: Implement popupUrl() method.
	}


}
