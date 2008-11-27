<?php
class PXEngineAdminPopup extends PXEngineAdminIndex {
	function __construct() {
		parent::__construct();

		$types = PXRegistry::getApp()->types;
		$this->layout = new PXAdminHTMLLayout('popup', $types);
		PXRegistry::setLayout($this->layout);
	}

	function initModules() {
		$this->modules = PXRegistry::getApp()->getAvailableModules(PXRegistry::getUser()->level);
		$this->area = PXRegistry::getRequest()->getArea(current(array_keys($this->modules)));
	}

	function fillLayout() {
		$this->layout->setGetVarToSave('area', $this->area);
	}

	function runModules() {
		PXProfiler::begin('RUN MODULES');

		if (!PXRegistry::getUser()->isAdmin()) {
			return $this->layout->assignError('OUTER.CONTENT', 'Нет доступа');
		}

		if(!$this->checkArea($this->area)) {
			return;
		}

		$this->fillLayout();

		$module = $this->modules[$this->area];
		$instance = $module->getModule();

		$this->layout->append('OUTER.CONTENT', $instance->adminPopup());

		PXProfiler::end();
	}
}
?>
