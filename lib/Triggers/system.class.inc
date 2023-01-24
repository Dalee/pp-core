<?php

/**
 * Abstract system events trigger class
 */
abstract class PXAbstractSystemTrigger extends PXAbstractTrigger {

	/**
	 * Runs after user authorization check passes
	 *
	 * @param PXUser $currentUser the user
	 */
	public function onAfterUserAuth($currentUser) {
	}

	/**
	 * Runs after engine starts
	 *
	 * @param PP\Lib\Engine\AbstractEngine $currentEngine the engine
	 */
	public function onAfterEngineStart($currentEngine) {
	}

	/**
	 * Runs before module work starts
	 *
	 * @param PP\Lib\Engine\AbstractEngine $currentEngine current engine
	 * @param PXModuleDescription $moduleDescription the module
	 * @param mixed $eventData Info about the engine method to be called
	 */
	public function onBeforeModuleRun($currentEngine, $moduleDescription, mixed $eventData) {
	}

	/**
	 * Runs after module work finishes
	 *
	 * @param PP\Lib\Engine\AbstractEngine $currentEngine current engine
	 * @param PXModuleDescription $moduleDescription the module
	 * @param mixed $eventData Info about the called engine method
	 */
	public function onAfterModuleRun($currentEngine, $moduleDescription, mixed $eventData) {
	}
}

