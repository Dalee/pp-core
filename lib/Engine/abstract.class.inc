<?php
abstract class PXEngine {
	protected $modules;
	protected $area;

	protected $app       = array('factory' => 'PXApplication', 'helper' => true);
	protected $request   = array('factory' => 'PXRequest');
	protected $db        = array('factory' => 'PXDatabase',    'helper' => true);
	protected $layout    = array('factory' => 'PXLayoutNull');
	protected $user      = array('factory' => 'PXUserAuthorized');

	protected $initOrder = array('app', 'db', 'request', 'user', 'layout');

	function __construct() {     // Инициализация основных объектов приложения
		PXProfiler::begin('INIT');

		$this->initApplication();

		$this->saveToRegistry();

		$this->user->setDb($this->db);
		$this->user->setApp($this->app);
		$this->user->setRequest($this->request);

		$this->user->checkAuth();
		$this->db->LoadDirectoriesAutomatic($this->app->directory);
		$this->initModules();

		PXProfiler::end();
	}

	protected function saveToRegistry() {
		foreach(array_keys(get_object_vars($this)) as $var){
			if(is_object($this->$var) && PXRegistry::canSaveIt($var)) {
				call_user_func_array(array("PXRegistry", 'set'.ucfirst($var)), array(&$this->$var));
			}
		}
	}

	function initApplication() {
		foreach($this->initOrder as $var){
			if(!is_array($this->$var) || empty($this->{$var}['factory'])){
				continue;
			}

			class_exists($this->{$var}['factory']) or FatalError("Factory class '{$this->{$var}['factory']}' for '{$var}' is not exists !");

			if($initHelper = (!empty($this->{$var}['helper']) && method_exists($this, 'init'.ucfirst($var)) ? 'init'.ucfirst($var) : false)){
				$this->$initHelper($this->{$var}['factory']);
			} else {
				$this->$var = & new $this->{$var}['factory'];
			}
		}
	}

	function initApp($klass){
		$this->app =& PXApplication::getInstance(get_class($this)); //$klass::getInstance(...) available since php 5.3.0 only
	}

	function initDb($klass){
		$this->db =& new $klass($this->app);
	}

	function initModules() {
		$rArea = $this->request->getVar('area');

		if(!isset($this->app->modules[$rArea])) {
			Finalize('/');
		}

		$this->modules = $this->app->modules;
		$this->area = $rArea;
	}

	function profilerStart() {
		PXProfiler::begin('TOTAL');
	}

	function profilerStop() {
		PXProfiler::end();

		$this->layout->assign('PXProfilerResult', PXProfiler::finalize());
	}

	abstract function runModules();
}
?>