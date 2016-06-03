<?php

namespace PP\Lib\Engine;

use PXApplication;
use PP\Lib\Html\Layout\LayoutInterface;
use Symfony\Component\HttpFoundation\Session\Session;

abstract class AbstractEngine {

	/** @var \PXModuleDescription[] */
	protected $modules;

	/** @var string */
	protected $area;

	/** @var \PXApplication */
	protected $app = array('factory' => 'PXApplication', 'helper' => true);

	/** @var \PXRequest */
	protected $request = array('factory' => 'PXRequest');

	/** @var \PXDatabase */
	protected $db = array('factory' => 'PXDatabase', 'helper' => true);

	/** @var LayoutInterface */
	protected $layout = array('factory' => 'PP\Lib\Html\Layout\Null');

	/** @var \PXUserAuthorized */
	protected $user = array('factory' => 'PXUserAuthorized');

	/**
	 * Should be initialized only in AbstractAdminEngine
	 *
	 * @var Session
	 */
	protected $session;

	protected $initOrder = array('app', 'db', 'request', 'user', 'layout');


	// TODO: refactor
	function __construct() {
		$this->initApplication();
		$this->saveToRegistry();
	}

	/**
	 *
	 */
	public function start() {

		$this->user->setDb($this->db);
		$this->user->setApp($this->app);
		$this->user->setRequest($this->request);
		$this->user->checkAuth();

		/** @var \PXTriggerDescription $t */
		foreach ($this->app->triggers->system as $t) {

			/** @var \PXAbstractSystemTrigger $trigger */
			$trigger = $t->getTrigger();
			$trigger->OnAfterEngineStart($this);
		}

		$this->db->LoadDirectoriesAutomatic($this->app->directory);
		$this->initModules();
	}

	protected function initApplication() {
		foreach ($this->initOrder as $var) {
			if (!is_array($this->$var) || empty($this->{$var}['factory'])) {
				continue;
			}

			class_exists($this->{$var}['factory']) or \FatalError("Factory class '{$this->{$var}['factory']}' for '{$var}' is not exists !");

			$initHelper = (!empty($this->{$var}['helper']) && method_exists($this, 'init' . ucfirst($var)) ? 'init' . ucfirst($var) : false);

			if ($initHelper) {
				$this->$initHelper($this->{$var}['factory']);
			} else {
				$this->$var = new $this->{$var}['factory'];
			}

		}
	}

	protected function saveToRegistry() {
		foreach (array_keys(get_object_vars($this)) as $var) {
			if (is_object($this->$var) && \PXRegistry::canSaveIt($var)) {
				call_user_func_array(array("PXRegistry", 'set' . ucfirst($var)), array(&$this->$var));
			}
		}
	}

	protected function initApp($klass) {
		$this->app = PXApplication::getInstance($this); //$klass::getInstance(...) available since php 5.3.0 only
	}

	protected function initDb($klass) {
		$this->db = new $klass($this->app);
	}

	protected function initModules() {
		$rArea = $this->request->getArea();

		if (!isset($this->app->modules[$rArea])) {
			Finalize('/');
		}

		$this->modules = $this->app->modules;
		$this->area = $rArea;
	}

	abstract function runModules();

	public function engineClass() {
		return PX_ENGINE_USER;
	}

	/**
	 * @return null|Session
	 */
	public function getSession() {
		return $this->session;
	}

	protected function checkArea($area) {
		if (!isset($this->modules[$area])) {
			FatalError('Некорректный параметр area = <em>' . strip_tags($this->area) . '</em>');
		}
	}

}
