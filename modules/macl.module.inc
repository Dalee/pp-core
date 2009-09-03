<?php
require_once 'acl.module.inc';

class PXModuleMAcl extends PXModuleAcl {
	var $what;
	var $access;
	var $objectRule     = 'module';
	var $moduleName     = 'macl';
	var $aclObjectTitle = 'Модуль';

	function __construct($area, $settings) {
		parent::__construct($area, $settings);
	}
	
	function getAvailableActions() {
		$actions = array('default' => PXModule::getAclModuleActions());
		
		foreach($this->app->modules as $name => $module) {
        	require_once $module->getPathToClass();

        	$actions[$module->name] = call_user_func(array($module->class, 'getAclModuleActions'));
        }
        return $actions;
	}
	
	function _getTypes() {
		$types = array();
		foreach($this->app->modules as $module) {
			$types[$module->name] = $module->description == '' || $module->description == PXModuleDescription::EMPTY_DESCRIPTION ? $module->name : $module->description;
		}

		$types[null] = '-- любой --';
		
		return $types;
	}
	
	function getWhatDict() {
		return $this->what['default'];
	}
	
	function adminJson() {
		$current = trim($this->request->getVar('currentModule'));
		
		if (isset($this->what[$current])) {
			$result = $this->what[$current];
		}
		else {
			$result = $this->what['default'];
		}
		
		return $result;
	}
}
?>