<?php

namespace PP\Lib\UrlGenerator;

class AdminUrlGenerator extends AbstractUrlGenerator {

	/**
	 * @param array [string]string $params
	 * @return string
	 */
	public function indexUrl($params = []) {
		$url = '/admin/';
		$params['area'] = $this->getArea();
		return $this->generateUrl($url, $params);
	}

	/**
	 * @param array [string]string $params
	 * @return string
	 */
	public function actionUrl($params = []) {
		$url = '/admin/action.phtml';
		$params['area'] = $this->getArea();

		if ($this->context->hasRequest()) {
			$sid = $this->context->getRequest()->getSid();
			if (!empty($sid)) {
				$params['sid'] = $sid;
			}
		}

		return $this->generateUrl($url, $params);
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
