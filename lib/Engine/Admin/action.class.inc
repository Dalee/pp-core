<?php
class PXEngineAdminAction extends PXEngine {
	var $nextLocation;

	function initModules() {
		$this->modules = $this->app->getAvailableModules();
		$this->area    = $this->request->getArea(current(array_keys($this->modules)));
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

		switch ($this->request->getAfterActionDeal()) {
			case 'close':
				closeAndRefresh();
				break;

			case 'back':
				$nextLocation = $this->nextLocation;
				$nextLocation = !is_null($nextLocation) ? $nextLocation : $this->request->getReferer();

				$response->redirect($nextLocation);
				break;

			default:
				break;
		}
	}
}
?>