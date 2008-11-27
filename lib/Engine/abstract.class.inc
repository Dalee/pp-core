<?php
abstract class PXEngine {
	protected $modules;
	protected $area;

	function __construct() {     // Инициализация основных объектов приложения
		PXProfiler::begin('INIT');

		$user = PXRegistry::getUser();

		$user->setAcl();
		$user->checkAuth();

		PXRegistry::getDb()->LoadDirectoriesAutomatic(PXRegistry::getApp()->directory);

		$this->initModules();

		PXProfiler::end();
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
