<?php

require_once PPPATH . 'modules/acl.module.inc';

class PXModuleMAcl extends PXModuleAcl {
	var $what;
	var $access;
	var $objectRule     = 'module';
	var $moduleName     = 'macl';
	var $aclObjectTitle = 'Модуль';

	function getAvailableActions() {
		$actions = array('default' => PXModule::getAclModuleActions());

		foreach($this->app->modules as $name => $module) {
			if (!class_exists($module->class, false)) {
				$path = $module->getPathToClass();

				if(!mb_strlen($path) && !file_exists($path)) {
					FatalError(sprintf("Can't find file with '%s' module", $name));
				}

				require_once $path;
			}

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
		} else {
			$result = $this->what['default'];
		}

		return $result;
	}
}
