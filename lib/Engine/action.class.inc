<?php

use Symfony\Component\DependencyInjection\ContainerAwareInterface;
use PP\Lib\Engine\AbstractEngine;

class PXEngineAction extends AbstractEngine {

	/** @var string */
	protected $nextLocation;

	function runModules() {
		$instance = $this->modules[$this->area]->getModule();

		$response = PXResponse::getInstance();
		$response->dontCache();

		if ($instance instanceof ContainerAwareInterface) {
			$instance->setContainer($this->container);
		}

		$this->nextLocation = $instance->userAction();
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
