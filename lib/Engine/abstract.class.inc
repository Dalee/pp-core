<?php
abstract class PXEngine {
	protected $modules;
	protected $area;

	function __construct() {     // Инициализация основных объектов приложения
		NLProfiler('INIT', 'BEGIN');

		$user = PXRegistry::getUser();

		$user->setDb(PXRegistry::getDb());
		$user->setApp(PXRegistry::getApp());
		$user->setRequest(PXRegistry::getRequest());
		$user->checkAuth();

		PXRegistry::getDb()->LoadDirectoriesAutomatic(PXRegistry::getApp()->directory);

		$this->initModules();

		NLProfiler('INIT', 'END');
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
