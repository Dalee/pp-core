<?php

namespace PP\Module;

use Symfony\Component\DependencyInjection\ContainerAwareTrait;

/**
 * Class AbstractModule.
 *
 * @package PP\Module
 */
abstract class AbstractModule implements ModuleInterface {

	use ContainerAwareTrait;

	var $area;
	var $settings;
	protected $__selfDescription;

	/**
	 * @var \PXApplication
	 */
	var $app;

	/**
	 * @var \PXDatabase|\PP\Lib\Database\Driver\PostgreSqlDriver
	 */
	var $db;

	/**
	 * @var \PXRequest
	 */
	var $request;

	/**
	 * @var \PXUser|\PXUserAuthorized
	 */
	var $user;

	/**
	 * @var \PP\Lib\Html\Layout\LayoutAbstract|\PP\Lib\Html\Layout\AdminHtmlLayout
	 */
	var $layout;

	/**
	 * @var \PP\Lib\Http\Response
	 */
	var $response;

	function __construct($area, $settings, $selfDescription = null) {
		$this->area = $area;
		$this->settings = $settings;
		$this->__selfDescription = $selfDescription; //for module acl checks purposes

		\PXRegistry::assignToObject($this);
	}

	/**
	 * @return array
	 */
	public static function getAclModuleActions() {
		$app = \PXRegistry::getApp();

		return [
			'viewmenu' => $app->langTree->getByPath('module_acl_rules.actions.viewmenu.rus'),
			'admin' => $app->langTree->getByPath('module_acl_rules.actions.madmin.rus')
		];
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
