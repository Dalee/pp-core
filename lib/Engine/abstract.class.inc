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

		$this->saveToRegistry();

		PXRegistry::getDb()->LoadDirectoriesAutomatic(PXRegistry::getApp()->directory);

		$this->initModules();

		PXProfiler::end();
	}

	function saveToRegistry() {
		foreach(array_keys(get_object_vars($this)) as $var){
			if(isset($this->$var) && PXRegistry::canSaveIt($var)){
				call_user_func_array(array("PXRegistry", 'set'.ucfirst($var)), array(&$this->$var));
			}
		}
	}

	function initApplication() { 
		$this->app =PXApplication::getInstance(get_class($this)); 
		PXRegistry::setApp($this->app); 
	}

	function initModules() {
		$rArea = $this->request->getVar('area');

		if(!isset(PXRegistry::getApp()->modules[$rArea])) {
			Finalize('/');
		}

		$this->modules = PXRegistry::getApp()->modules;
		$this->area = $rArea;
	}

	abstract function runModules();
}
?>
