<?php

use PP\Lib\Module\ModuleInterface;

abstract class PXModule implements ModuleInterface {
	var $area;
	var $settings;
	protected $__selfDescription;

	/**
	 * @var PXApplication
	 */
	var $app;

	/**
	 * @var PXDatabase|NLPGSQLDatabase
	 */
	var $db;

	/**
	 * @var PXRequest
	 */
	var $request;

	/**
	 * @var PXUser|PXUserAuthorized
	 */
	var $user;

	/**
	 * @var \PP\Lib\Html\Layout\LayoutAbstract|\PP\Lib\Html\Layout\AdminHtmlLayout
	 */
	var $layout;

	/**
	 * @var PXResponse
	 */
	var $response;

	function __construct($area, $settings, $selfDescription = null) {
		$this->area = $area;
		$this->settings = $settings;
		$this->__selfDescription = $selfDescription; //for module acl checks purposes

		PXRegistry::assignToObject($this);
	}

	/**
	 * @return array
	 */
	public static function getAclModuleActions() {
		$app = PXRegistry::getApp();

		return [
			'viewmenu' => $app->langTree->getByPath('module_acl_rules.actions.viewmenu.rus'),
			'admin' => $app->langTree->getByPath('module_acl_rules.actions.madmin.rus')
		];
	}

	function buildAdminUrl() {
		return str_replace('action.phtml', '', $_SERVER['SCRIPT_URL']);
	}

	function buildAdminIndexUrl() {
		return str_replace('action.phtml', '', $_SERVER['SCRIPT_URL']) . '?area=' . $this->area;
	}

	/**
	 * @param string $action
	 * @param bool $addIdParam
	 * @return string
	 */
	protected function buildAdminActionUrl($action = 'main', $addIdParam = false) {
		$link = sprintf('/admin/action.phtml?area=%s&action=%s', $this->area, urlencode($action));
		$sid = $this->request->getSid();
		if (!empty($sid)) {
			$link = $link.'&sid='.urlencode($sid);
		}
		if ($addIdParam) {
			$link = $link.'&id=';
		}
		return $link;
	}

	/**
	 * @param string $action
	 * @param bool $addIdParam
	 * @return string
	 */
	protected function buildAdminPopupUrl($action = 'main', $addIdParam = false) {
		$link = sprintf('/admin/popup.phtml?area=%s&action=%s', $this->area, urlencode($action));
		$sid = $this->request->getSid();
		if (!empty($sid)) {
			$link = $link.'&sid='.urlencode($sid);
		}
		if ($addIdParam) {
			$link = $link.'&id=';
		}

		return $link;
	}

	/**
	 * {@inheritdoc}
	 */
	function adminIndex() {
		$this->layout->assignError('INNER.1.0', 'Функция <em>adminIndex</em> данного модуля не определена');
	}

	/**
	 * {@inheritdoc}
	 */
	function adminPopup() {
		$this->layout->assignError('OUTER.CONTENT', 'Функция <em>adminPopup</em> данного модуля не определена');
	}

	/**
	 * {@inheritdoc}
	 */
	function adminAction() {
		FatalError("Функция <em>adminAction</em> данного модуля не определена");
	}

	/**
	 * {@inheritdoc}
	 */
	function userIndex() {
	}

	/**
	 * {@inheritdoc}
	 */
	function userAction() {
	}

	/**
	 * {@inheritdoc}
	 */
	function userJson() {
	}

	/**
	 * {@inheritdoc}
	 */
	public function adminJson() {
	}
}
