<?php

namespace PP\Module;

/**
 * Class MaclModule.
 *
 * @package PP\Module
 */
class MaclModule extends AclModule
{
    public $what;
    public $access;
    public $objectRule = 'module';
    public $moduleName = 'macl';
    public $aclObjectTitle = 'Модуль';

    public function getAvailableActions()
    {
        $actions = ['default' => AbstractModule::getAclModuleActions()];

        foreach ($this->app->modules as $name => $module) {
            $actions[$module->getName()] = call_user_func([$module->getClass(), 'getAclModuleActions']);
        }

        return $actions;
    }

    public function _getTypes()
    {
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

    public function getWhatDict()
    {
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

    public function adminJson()
    {
        $current = trim((string) $this->request->getVar('currentModule'));

        return $this->what[$current] ?? $this->what['default'];
    }

}
