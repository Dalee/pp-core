<?php

use PP\Lib\Http\Response;
use PP\DependencyInjection\ContainerAwareInterface;
use PP\Lib\Engine\AbstractEngine;

class PXEngineIndex extends AbstractEngine
{
    public $layout = ['factory' => 'PXUserHTMLLayout'];

    /**
     * Инициализация основных объектов приложения
     * Initializes application's main objects
     */
    public function __construct()
    {
        parent::__construct();

        $this->content = new stdClass(); // FIXME
        $this->content->tree = new PXTreeObjects();
        $this->content->objects = new PXObjects();
        $this->content->subObjects = new PXSubObjects();

        $this->layout->setApp($this->app);
    }

    protected function initModules()
    {
    }

    public function fillLayout()
    {
        $this->layout->setApp(PXRegistry::getApp());

        // Передача данных в шаблонизатор
        $this->layout->assign('app', PXRegistry::getApp(), true);
        $this->layout->assign('user', PXRegistry::getUser(), true);
        $this->layout->assign('request', PXRegistry::getRequest(), true);
        $this->layout->assign('response', Response::getInstance(), true);

        $this->layout->assignRequest(PXRegistry::getRequest());

        $this->layout->assign('tree', $this->content->tree);
        $this->layout->assign('objects', $this->content->objects);
        $this->layout->assign('subObjects', $this->content->subObjects);

        // Deprecated features
        $currentSid = $this->content->tree->hasCurrent() ? $this->content->tree->getCurrent()->id : -1;
        if ($this->content->objects->hasCurrent()) {
            $objs = $this->content->objects->getCurrent();
            $currentCid = $objs['id'];
        } else {
            $currentCid = -1;
        }

        if ($this->content->subObjects->hasCurrent()) {
            $objs = $this->content->subObjects->getCurrent();
            $currentSCid = $objs['id'];
        } else {
            $currentSCid = -1;
        }

        $this->layout->assign('currentSid', $currentSid);
        $this->layout->assign('currentCid', $currentCid);
        $this->layout->assign('currentSCid', $currentSCid);

        $this->layout->assign('currentCtype', $this->content->objects->getCurrentType());

        $this->layout->assign('pathId', $this->content->tree->pathId);
        $this->layout->assign('rootId', $this->content->tree->rootId);
        $this->layout->assign('urlFile', PXRegistry::getRequest()->getFile());

        $this->layout->assign('requestHost', PXRegistry::getRequest()->getHttpHost());
        $this->layout->assign('requestUri', PXRegistry::getRequest()->getRequestUri());
        $this->layout->assign('requestReferer', PXRegistry::getRequest()->getHttpReferer());
        $this->layout->assign('requestPath', PXRegistry::getRequest()->getPathAsString());
        $this->layout->assign('REGEX_MOD', REGEX_MOD);
        $this->layout->assign('DEFAULT_CHARSET', DEFAULT_CHARSET);
        $this->layout->assign('CHARSET_UTF8', CHARSET_UTF8);
        $this->layout->assign('CHARSET_WINDOWS', CHARSET_WINDOWS);
    }

    public function html()
    {
        $html = PXRegistry::getLayout()->display();

        $response = Response::getInstance();
        $response->send($html);
    }

    public function runModules()
    {
        $queue = PXRegistry::getApp()->bindingsQueue;
        $queue->getRequestBinding();

        foreach ($queue as $bind) {
            if (!$bind->isBinding($this->content)) {
                continue;
            }

            $moduleDescription = $bind->getModuleDescription();
            $instance = $moduleDescription->getModule();

            if ($instance instanceof ContainerAwareInterface) {
                $instance->setContainer($this->container);
            }

            $instance->tree =& $this->content->tree;
            $instance->objects =& $this->content->objects;
            $instance->subObjects =& $this->content->subObjects;

            $eventData = [
                'engine_type' => $this->engineType(),
                'engine_behavior' => $this->engineBehavior(),
            ];
            foreach ($this->app->triggers->system as $t) {
                $t->getTrigger()->onBeforeModuleRun($this, $moduleDescription, $eventData);
            }

            $instance->userIndex();

            foreach ($this->app->triggers->system as $t) {
                $t->getTrigger()->onAfterModuleRun($this, $moduleDescription, $eventData);
            }
        }
    }

    /** {@inheritdoc} */
    public function engineType()
    {
        return static::USER_ENGINE_TAG;
    }

    /** {@inheritdoc} */
    public function engineBehavior()
    {
        return static::INDEX_BEHAVIOR;
    }
}

