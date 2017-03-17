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
		$params['area'] = $this->getArea();
		return $this->generateUrl($url, $params);
	}

	/**
	 * @inheritdoc
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
	 * @inheritdoc
	 */
	public function jsonUrl($params = []) {
		// TODO: Implement jsonUrl() method.
	}

	/**
	 * @inheritdoc
	 */
	public function popupUrl($params = []) {
		// TODO: Implement popupUrl() method.
	}

}
