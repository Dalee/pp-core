<?php

namespace PP\Module;

/**
 * Class MaclModule.
 *
 * @package PP\Module
 */
class MaclModule extends AclModule {

	var $what;
	var $access;
	var $objectRule     = 'module';
	var $moduleName     = 'macl';
	var $aclObjectTitle = 'Модуль';

	function getAvailableActions() {
		$actions = ['default' => \PXModule::getAclModuleActions()];

		foreach($this->app->modules as $name => $module) {
			$actions[$module->getName()] = call_user_func([$module->getClass(), 'getAclModuleActions']);
		}

		return $actions;
	}

	function _getTypes() {
		$types = [];

		foreach ($this->app->modules as $module) {
			if ($module->getDescription() == '' || $module->getDescription() == \PXModuleDescription::EMPTY_DESCRIPTION) {
				$types[$module->getName()] = $module->getName();
			} else {
				$types[$module->getName()] = $module->getDescription();
			}
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
