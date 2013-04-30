<?php
class PXEngineAdminPopup extends PXEngineAdminIndex {
	var $outerLayout            = 'popup';
	protected $templateMainArea = 'OUTER.CONTENT';

	function initModules() {
		$this->area    = $this->request->getArea();
		$this->modules = self::getModule($this->app, $this->area);
	}

	function fillLayout($area = NULL) { /* E_STRICT */
		$this->layout->setGetVarToSave('area', $this->area);
	}

	function runModules() {
		PXProfiler::begin('RUN MODULES');

		if (!$this->hasAdminModules()) {
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
