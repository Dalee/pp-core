<?php
class PXEngineAdminAction extends PXEngineAdmin {
	var $nextLocation;

	function initModules() {
		$this->area    = $this->request->getArea();
		$this->modules = array($this->area => $this->app->getAvailableModule($this->area));
	}

	function runModules() {
		PXProfiler::begin('RUN MODULES');

		// For correct user login expiration handling
		if (!$this->user->isAdmin()) {
			return;
		}

		$this->checkArea($this->area);

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
