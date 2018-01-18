<?php

use Symfony\Component\DependencyInjection\ContainerAwareInterface;
use PP\Lib\Engine\AbstractEngine;

class PXEngineAction extends AbstractEngine {

	/** @var string */
	protected $nextLocation;

	function runModules() {
		$moduleDescription = $this->modules[$this->area];
		$instance = $moduleDescription->getModule();

		$response = PXResponse::getInstance();
		$response->dontCache();

		if ($instance instanceof ContainerAwareInterface) {
			$instance->setContainer($this->container);
		}

		$eventData = [
			'engine_type' => $this->engineType(),
			'engine_behavior' => $this->engineBehavior()
		];
		foreach ($this->app->triggers->system as $t) {
			$t->getTrigger()->onBeforeModuleRun($this, $moduleDescription, $eventData);
		}

		$this->nextLocation = $instance->userAction();

		foreach ($this->app->triggers->system as $t) {
			$t->getTrigger()->onAfterModuleRun($this, $moduleDescription, $eventData);
		}
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

	/** {@inheritdoc} */
	public function engineType() {
		return static::USER_ENGINE_TAG;
	}

	/** {@inheritdoc} */
	public function engineBehavior() {
		return static::ACTION_BEHAVIOR;
	}

}
