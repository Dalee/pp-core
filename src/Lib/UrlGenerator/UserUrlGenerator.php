<?php

namespace PP\Lib\UrlGenerator;

/**
 * Class UserUrlGenerator
 * @package PP\Lib\UrlGenerator
 */
class UserUrlGenerator extends AbstractUrlGenerator {

	/**
	 * @inheritdoc
	 */
	public function indexUrl($params = []) {
		throw new \LogicException('You cannot use the method: ' . __METHOD__);
	}

	/**
	 * @inheritDoc
	 */
	public function actionUrl($params = []) {
		$url = '/' . $this->getArea() . '.action';
		return $this->generateUrl($url, $params);
	}

	/**
	 * @inheritDoc
	 */
	public function jsonUrl($params = []) {
		$url = '/' . $this->getArea() . '.json';
		return $this->generateUrl($url, $params);
	}

	/**
	 * @inheritDoc
	 */
	public function popupUrl($params = []) {
		throw new \LogicException('You cannot use the method: ' . __METHOD__);
	}
}
