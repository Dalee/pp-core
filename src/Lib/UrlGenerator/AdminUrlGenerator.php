<?php

namespace PP\Lib\UrlGenerator;

/**
 * Class AdminUrlGenerator
 * @package PP\Lib\UrlGenerator
 */
class AdminUrlGenerator extends AbstractUrlGenerator {

	/**
	 * {@inheritdoc}
	 */
	public function indexUrl($params = []) {
		$url = '/admin/';
		$oldParams['area'] = $this->getArea();
		$params = array_replace($oldParams, $params);
		return $this->generateUrl($url, $params);
	}

	/**
	 * @inheritdoc
	 */
	public function actionUrl($params = []) {
		$url = '/admin/action.phtml';
		$oldParams['area'] = $this->getArea();
		$sid = $this->getSid();
		if ($sid !== null) {
			$oldParams['sid'] = $sid;
		}
		$params = array_replace($oldParams, $params);
		return $this->generateUrl($url, $params);
	}

	/**
	 * @inheritdoc
	 */
	public function jsonUrl($params = []) {
		$url = '/admin/json.phtml';
		$oldParams['area'] = $this->getArea();
		$params = array_replace($oldParams, $params);
		return $this->generateUrl($url, $params);
	}

	/**
	 * @inheritdoc
	 */
	public function popupUrl($params = []) {
		$url = '/admin/popup.phtml';
		$oldParams['area'] = $this->getArea();
		$sid = $this->getSid();
		if ($sid !== null) {
			$oldParams['sid'] = $sid;
		}
		$params = array_replace($oldParams, $params);
		return $this->generateUrl($url, $params);
	}

}
