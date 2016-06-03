<?php

namespace PP\Lib\Engine\Admin;

use PP\Lib\Session\DatabaseHandler;
use PXApplication;
use PP\Lib\Engine\AbstractEngine;
use Symfony\Component\HttpFoundation\Session\Session;
use Symfony\Component\HttpFoundation\Session\Storage\NativeSessionStorage;


abstract class AbstractAdminEngine extends AbstractEngine {

	const SESSION_NAME = 'sid';

	// TODO: it is better if auth module will export
	// info about its authorizable behaviour, like this -> (bool)$module->thisIsAdminAuthModule() ?
	protected $authArea = 'auth';


	function __construct() {
		parent::__construct();

		$storage = new NativeSessionStorage([], new DatabaseHandler($this->db));
		$this->session = new Session($storage);
		$this->session->setName(static::SESSION_NAME);
		$this->session->start();
	}

	public function engineClass() {
		return PX_ENGINE_ADMIN;
	}

	// static because PXEngineAdminJSON inherited from PXEngineJSON (stupid, but traits available only from php 5.4)
	public static function getModule(PXApplication $app, $area) {
		return array_filter(array($area => $app->getAvailableModule($area)));
	}

	protected function hasAdminModules() {
		// $user->isAdmin() is obsolete for Admin Engines,
		// because $this->modules are filtered with $user->can('admin'...) before
		return count($this->modules) > 1 || (count($this->modules) == 1 && !isset($this->modules[$this->authArea]));
	}
}
