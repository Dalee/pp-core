<?php
abstract class PXEngine {
	protected $modules;
	protected $area;

	function __construct() {     // Инициализация основных объектов приложения
		PXProfiler::begin('INIT');

		$this->initApplication();
		$user = PXRegistry::getUser();

		$user->setAcl();
		$user->checkAuth();

		PXRegistry::getDb()->LoadDirectoriesAutomatic(PXRegistry::getApp()->directory);

		$this->initModules();

		PXProfiler::end();
	}

	function initApplication() { 
		$this->app =PXApplication::getInstance(get_class($this)); 
		PXRegistry::setApp($this->app); 
	}

	function initModules() {
		$rArea = $request->getVar('area');

		if(!isset(PXRegistry::getApp()->modules[$rArea])) {
			Finalize('/');
		}

		$this->modules = PXRegistry::getApp()->modules;
		$this->area = $rArea;
	}

	abstract function runModules();
}
?>
