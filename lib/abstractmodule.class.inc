<?php
interface PXModuleIF {
	function adminIndex();
	function adminAction();
	function adminPopup();
	function userIndex();
	function userAction();
	function userJson();
	function adminJson();
}

abstract class PXModule implements PXModuleIF {
	var $area;
	var $settings;
	protected $__selfDescription;

	var $app;
	var $db;
	var $request;
	var $user;
	var $layout;
	var $response;

	function __construct($area, $settings, $selfDescription = null) {
		$this->area              = $area;
		$this->settings          = $settings;
		$this->__selfDescription = $selfDescription; //for module acl checks purposes

		PXRegistry::assignToObject($this);
	}

	function getAclModuleActions() {
		return array('viewmenu' => 'Показывать в меню', 'admin' => 'Доступ в админке');
	}

	function buildAdminUrl() {
		return str_replace('action.phtml', '', $_SERVER['SCRIPT_URL']);
	}

	function buildAdminIndexUrl() {
		return str_replace('action.phtml', '', $_SERVER['SCRIPT_URL']).'?area='.$this->area;
	}

	function adminIndex() {
		$this->layout->assignError('INNER.1.0', 'Функция <em>adminIndex</em> данного модуля не определена');
	}

	function adminPopup() {
		$this->layout->assignError('OUTER.CONTENT', 'Функция <em>adminPopup</em> данного модуля не определена');
	}

	function adminAction() {
		FatalError("Функция <em>adminAction</em> данного модуля не определена");
	}

	function userIndex() {
	}

	function userAction() {
	}

	function userJson() {
	}
	
	function adminJson() {
	}
	
	function finalizeBindingsQueue() {
		$this->app->bindingsQueue->stopLoading();
	}
}

abstract class PXAbstractModule extends PXModule implements PXModuleIF {
	function __call($method, $args) {
		throw new DeprecatedException("PXAbstractModule is deprecated. Use PXModule");
	}
}
?>
