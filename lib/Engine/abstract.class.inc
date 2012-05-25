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

	function __construct() { // Инициализация основных объектов приложения
		PXProfiler::begin('INIT');

		$this->initApplication();

		$this->saveToRegistry();

		$this->user->setDb($this->db);
		$this->user->setApp($this->app);
		$this->user->setRequest($this->request);

		$this->user->checkAuth();

		foreach ($this->app->triggers->system as $t) {
			$t->getTrigger()->OnAfterEngineStart($this); //FIXME: this line is correct placeholder for this event ?
		}

		$this->db->LoadDirectoriesAutomatic($this->app->directory);
		$this->initModules();

		PXProfiler::end();
	}

	protected function saveToRegistry() {
		foreach (array_keys(get_object_vars($this)) as $var) {
			if (is_object($this->$var) && PXRegistry::canSaveIt($var)) {
				call_user_func_array(array("PXRegistry", 'set'.ucfirst($var)), array(&$this->$var));
			}
		}
	}

	protected function initApplication() {
		foreach ($this->initOrder as $var) {
			if (!is_array($this->$var) || empty($this->{$var}['factory'])) {
				continue;
			}

			class_exists($this->{$var}['factory']) or FatalError("Factory class '{$this->{$var}['factory']}' for '{$var}' is not exists !");

			$initHelper = (!empty($this->{$var}['helper']) && method_exists($this, 'init'.ucfirst($var)) ? 'init'.ucfirst($var) : false);

			if ($initHelper) {
				$this->$initHelper($this->{$var}['factory']);
			} else {
				$this->$var = new $this->{$var}['factory'];
			}

		}
	}

	protected function initApp($klass) {
		$this->app = PXApplication::getInstance($this); //$klass::getInstance(...) available since php 5.3.0 only
	}

	protected function initDb($klass) {
		$this->db = new $klass($this->app);
	}

	function initModules() {
		$rArea = $this->request->getArea();

		if (!isset($this->app->modules[$rArea])) {
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

	public function engineClass() {
		return PX_ENGINE_USER;
	}

	function checkArea($area) {
		if (!isset($this->modules[$area])) {
			FatalError('Некорректный параметр area = <em>' . strip_tags($this->area) . '</em>');
		}
	}
}

?>
