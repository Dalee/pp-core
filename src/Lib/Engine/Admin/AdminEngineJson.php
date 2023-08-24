<?php

namespace PP\Lib\Engine\Admin;

use PP\Lib\Http\Response;
use Symfony\Component\DependencyInjection\ContainerAwareInterface;

/**
 * Class AdminEngineJson.
 *
 * @package PP\Lib\Engine\Admin
 */
class AdminEngineJson extends AbstractAdminEngine
{
    protected $result;

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

        $this->result = $instance->adminJson();

        foreach ($this->app->triggers->system as $t) {
            $t->getTrigger()->onAfterModuleRun($this, $moduleDescription, $eventData);
        }
    }

    public function sendJson(): void
    {
        $response = Response::getInstance();
        $response->sendJson($this->result);
        exit;
    }

    /** {@inheritdoc} */
    public function engineBehavior()
    {
        return static::JSON_BEHAVIOR;
    }
}
