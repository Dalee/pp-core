<?php

namespace PP\Lib\Engine\Admin;

use PP\Lib\Http\Response;
use PP\DependencyInjection\ContainerAwareInterface;

/**
 * Class AdminEngineAction.
 *
 * @package PP\Lib\Engine\Admin
 */
class AdminEngineAction extends AbstractAdminEngine
{
    public $nextLocation;

    public function initModules()
    {
        $this->area = $this->request->getArea();
        $this->modules = $this->getModule($this->app, $this->area);
    }

    public function runModules()
    {
        // For correct user session expiration handling and admin auth module working
        if (!($this->hasAdminModules() || $this->area == $this->authArea)) {
            return;
        }

        $this->checkArea($this->area);

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

        $this->nextLocation = $instance->adminAction();

        foreach ($this->app->triggers->system as $t) {
            $t->getTrigger()->onAfterModuleRun($this, $moduleDescription, $eventData);
        }
    }

    public function redirect()
    {
        $response = Response::getInstance();
        $response->dontCache();

        switch ($this->request->getAfterActionDeal()) {
            case 'close':
                \CloseAndRefresh();
                break;

            case 'back':
                $nextLocation = $this->nextLocation;
                $nextLocation = !is_null($nextLocation) ? $nextLocation : $this->request->getReferer();

                $response->redirect($nextLocation);
                break;

            default:
                break;
        }
    }

    /** {@inheritdoc} */
    public function engineBehavior()
    {
        return static::ACTION_BEHAVIOR;
    }
}
