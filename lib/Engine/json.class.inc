<?php

use PP\Lib\Http\Response;
use PP\DependencyInjection\ContainerAwareInterface;
use PP\Lib\Engine\AbstractEngine;
use PP\Module\AbstractModule;

class PXEngineJSON extends AbstractEngine
{
	protected $result;

	public function runModules()
	{
		$moduleDescription = $this->modules[$this->area];
		$instance = $moduleDescription->getModule();

		$response = Response::getInstance();
		$response->dontCache();

		if ($instance instanceof ContainerAwareInterface) {
			$instance->setContainer($this->container);
		}

		$eventData = [
			'engine_type' => $this->engineType(),
			'engine_behavior' => $this->engineBehavior(),
		];
		foreach ($this->app->triggers->system as $t) {
			$t->getTrigger()->onBeforeModuleRun($this, $moduleDescription, $eventData);
		}

		$this->result = $this->getJson($instance);

		foreach ($this->app->triggers->system as $t) {
			$t->getTrigger()->onAfterModuleRun($this, $moduleDescription, $eventData);
		}
	}

	public function sendJson(): void
	{
		$response = Response::getInstance();
		$response->sendJson($this->result);
		exit;
	}

	/**
	 * @param AbstractModule $module
	 * @return mixed
	 */
	public function getJson($module)
	{
		return $module->userJson();
	}

	// Common data encoders for library methods standardization

	public static function toError($value, $koi82utf8 = false)
	{
		return self::toObject('iserror', $value, $koi82utf8);
	}

	public static function toSuccess($value, $koi82utf8 = false)
	{
		return self::toObject('isok', $value, $koi82utf8);
	}

	protected static function toObject($key, $value, $encode)
	{
		return (object)[$key => (object)['notice' => $value]];
	}

	/** {@inheritdoc} */
	public function engineType()
	{
		return static::USER_ENGINE_TAG;
	}

	/** {@inheritdoc} */
	public function engineBehavior()
	{
		return static::JSON_BEHAVIOR;
	}
}
