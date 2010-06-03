<?php
class PXEngineAdminPopup extends PXEngineAdminIndex {
	var $outerLayout = 'popup';

	function initModules() {
		$this->modules = $this->app->getAvailableModules();
		$this->area    = $this->request->getArea(current(array_keys($this->modules)));
	}

	function fillLayout() {
		$this->layout->setGetVarToSave('area', $this->area);
	}

	function runModules() {
		PXProfiler::begin('RUN MODULES');

		if (!$this->user->isAdmin()) {
			return $this->layout->assignError('OUTER.CONTENT', 'Нет доступа');
		}

		if(!$this->checkArea($this->area)) {
			return;
		}

		$this->fillLayout();

		$instance = $this->modules[$this->area]->getModule();

		$this->layout->append('OUTER.CONTENT', $instance->adminPopup());
		

		PXProfiler::end();
	}
}
?>
