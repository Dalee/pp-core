<?php

use PP\Lib\Engine\AbstractEngine;

class PXEngineAction extends AbstractEngine {

	/** @var string */
	protected $nextLocation;

	function runModules() {
		$module = $this->modules[$this->area]->getModule();

		$response = PXResponse::getInstance();
		$response->dontCache();

		$this->nextLocation = $module->userAction();
	}

	function redirect() {
		$nextLocation = $this->nextLocation;

		if (empty($nextLocation)) {
			$nextLocation = $this->request->GetHttpReferer();
		}

		if (empty($nextLocation)) {
			$nextLocation = '/';
		}

		$response = PXResponse::getInstance();
		$response->redirect($nextLocation);
	}
}
