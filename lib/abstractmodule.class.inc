<?php
interface PXModuleIF {
	function adminIndex();
	function adminAction();
	function adminPopup();
	function userIndex();
	function userAction();
	function userJson();
}

abstract class PXModule implements PXModuleIF {
	function __construct($area, $settings) {
		$this->area       = $area;
		$this->settings   = $settings;

		$this->app        = PXRegistry::getApp();
		$this->request    = PXRegistry::getRequest();
		$this->user       = PXRegistry::getUser();
		$this->db         = PXRegistry::getDb();
		$this->layout     = PXRegistry::getLayout();
		$this->types      = PXRegistry::getTypes();
/*
TODO FIXME
		$this->struct     = PXRegistry::getVar('struct');
		$this->content    = PXRegistry::getVar('content');
		$this->subContent = PXRegistry::getVar('subContent');

		$this->tree       = $this->struct->tree;
		$this->objects    = $this->content->data;
		$this->subObjects = $this->subContent->data;
*/
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
}

abstract class PXAbstractModule extends PXModule implements PXModuleIF {
	function __call($method, $args) {
		throw new DeprecatedException("PXAbstractModule is deprecated. Use PXModule");
	}
}
?>
