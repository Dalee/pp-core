<?php
class PXEngineAdminPopup extends PXEngineAdminIndex {
	var $outerLayout            = 'popup';
	protected $templateMainArea = 'OUTER.CONTENT';

	function initModules() {
		$this->area    = $this->request->getArea();
		$this->modules = array($this->area => $this->app->getAvailableModule($this->area));
	}

	function fillLayout() {
		$this->layout->setGetVarToSave('area', $this->area);
	}

	function runModules() {
		PXProfiler::begin('RUN MODULES');

		if (!$this->user->isAdmin()) {
			return $this->layout->assignError($this->templateMainArea, 'Нет доступа');
		}

		if(!$this->checkArea($this->area)) {
			return;
		}

		$this->fillLayout();

		$instance = $this->modules[$this->area]->getModule();

		$this->layout->append($this->templateMainArea, $instance->adminPopup());
		

		PXProfiler::end();
	}
}
?>
