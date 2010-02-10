<?php
set_time_limit(0);
class PXEngineAdminAction extends PXEngine {
	var $nextLocation;

	function initLayout() {
		$this->layout = new PXLayoutNull();
	}

	function initModules() {
		$this->modules = PXRegistry::getApp()->getAvailableModules();
		$this->area    = PXRegistry::getRequest()->getArea(current(array_keys($this->modules)));
	}

	function runModules() {
		PXProfiler::begin('RUN MODULES');

		if (!isset($this->modules[$this->area])) {
			FatalError('Некорректный параметр area = <em>'.strip_tags($this->area).'</em>');
		}

		$instance = $this->modules[$this->area]->getModule();
		$this->nextLocation = $instance->adminAction();

		PXProfiler::end();
	}

	function redirect() {
		$response =& PXResponse::getInstance();
		$response->dontCache();

		switch (PXRegistry::getRequest()->getAfterActionDeal()) {
			case 'close':
				closeAndRefresh();
				break;

			case 'back':
				$nextLocation = $this->nextLocation;
				$nextLocation = !is_null($nextLocation) ? $nextLocation : PXRegistry::getRequest()->getReferer();

				$response->redirect($nextLocation);
				break;

			default:
				break;
		}
	}
}
?>