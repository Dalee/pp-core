<?php
abstract class PXEngineAdmin extends PXEngine {

	// TODO: it is better if auth module will export
	// info about its authorizable behaviour, like this -> (bool)$module->thisIsAdminAuthModule() ?
	protected $authArea = 'auth';

	public function engineClass() {
		return PX_ENGINE_ADMIN;
	}

	// static because PXEngineAdminJSON inherited from PXEngineJSON (stupid, but traits available only from php 5.4)
	public static function getModule($app, $area) {
		return array_filter(array($area => $app->getAvailableModule($area)));
	}

	protected function hasAdminModules() {
		// $user->isAdmin() is obsolete for Admin Engines,
		// because $this->modules are filtered with $user->can('admin'...) before
		return count($this->modules) > 1 || (count($this->modules) == 1 && !isset($this->modules[$this->authArea]));
	}
}
?>