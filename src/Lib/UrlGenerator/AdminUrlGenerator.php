<?php

namespace PP\Lib\UrlGenerator;

class AdminUrlGenerator extends AbstractUrlGenerator {

	/**
	 * @param array [string]string $params
	 * @return string
	 */
	public function indexUrl($params = []) {
		return str_replace('action.phtml', '', $_SERVER['SCRIPT_URL']) . '?area=' . $this->area;
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
