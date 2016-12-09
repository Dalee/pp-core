<?php

require_once PPPATH . 'modules/acl.module.inc';

class PXModuleMAcl extends PXModuleAcl {
	var $what;
	var $access;
	var $objectRule     = 'module';
	var $moduleName     = 'macl';
	var $aclObjectTitle = 'Модуль';

	function getAvailableActions() {
		$actions = ['default' => PXModule::getAclModuleActions()];

		foreach($this->app->modules as $name => $module) {
			if (!class_exists($module->class, false)) {
				$path = $module->getPathToClass();

				if(!mb_strlen($path) && !file_exists($path)) {
					FatalError(sprintf("Can't find file with '%s' module", $name));
				}

				require_once $path;
			}

			$actions[$module->name] = call_user_func([$module->class, 'getAclModuleActions']);
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
		$defaultActionList = $this->what['default'];
		$allActions = $this->getAvailableActions();

		foreach ($allActions as $moduleName => $moduleActions) {
			foreach ($moduleActions as $actionName => $actionTitle) {
				if (!isset($defaultActionList[$actionName])) {
					$defaultActionList[$actionName] = $actionTitle;
				}
			}
		}

		return $defaultActionList;
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
