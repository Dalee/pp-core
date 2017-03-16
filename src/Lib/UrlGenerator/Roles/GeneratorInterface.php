<?php

namespace PP\Lib\UrlGenerator\Roles;

interface GeneratorInterface {

	const ACTION_INDEX = 'index';
	const ACTION_ACTION = 'action';
	const ACTION_JSON = 'json';
	const ACTION_POPUP = 'popup';

	/**
	 * @param array[string]string $params
	 * @return string
	 */
	public function indexUrl($params = []);

	/**
	 * @param array[string]string $params
	 * @return string
	 */
	public function actionUrl($params = []);

	/**
	 * @param array[string]string $params
	 * @return string
	 */
	public function jsonUrl($params = []);

	/**
	 * @param array[string]string $params
	 * @return string
	 */
	public function popupUrl($params = []);

}
