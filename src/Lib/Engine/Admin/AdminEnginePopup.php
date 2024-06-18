<?php

namespace PP\Lib\Engine\Admin;

use PP\DependencyInjection\ContainerAwareInterface;

/**
 * Class AdminEnginePopup.
 *
 * @package PP\Lib\Engine\Admin
 */
class AdminEnginePopup extends AdminEngineIndex
{
    protected $outerLayout = 'popup';
    protected $templateMainArea = 'OUTER.CONTENT';

    public function initModules()
    {
        $this->area = $this->request->getArea();
        $this->modules = $this->getModule($this->app, $this->area);
    }

    public function fillLayout($area = null)
    {
        $this->layout->assignFlashes();
        $this->layout->setGetVarToSave('area', $this->area);
    }

    public function runModules()
    {
        if (!$this->hasAdminModules()) {
            $this->layout->assignError($this->templateMainArea, 'Нет доступа');
            return;
        }

        if (!$this->checkArea($this->area)) {
            return;
        }

        $this->fillLayout();
        $moduleDescription = $this->modules[$this->area];
        $instance = $moduleDescription->getModule();
        if ($instance instanceof ContainerAwareInterface) {
            $instance->setContainer($this->container);
        }

        $eventData = [
            'engine_type' => $this->engineType(),
            'engine_behavior' => $this->engineBehavior(),
        ];
        foreach ($this->app->triggers->system as $t) {
            $t->getTrigger()->onBeforeModuleRun($this, $moduleDescription, $eventData);
        }

        $popup = $instance->adminPopup();

        foreach ($this->app->triggers->system as $t) {
            $t->getTrigger()->onAfterModuleRun($this, $moduleDescription, $eventData);
        }

        $this->layout->append($this->templateMainArea, $popup);
    }

    /** {@inheritdoc} */
    public function engineBehavior()
    {
        return static::POPUP_BEHAVIOR;
    }
}
