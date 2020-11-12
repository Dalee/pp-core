<?php

use PP\Lib\Engine\AbstractEngine;
use PP\Lib\Engine\EngineInterface;

class PXTriggerSystemMultipleregions extends PXAbstractSystemTrigger
{
    private $engines = [
        EngineInterface::USER_ENGINE_ID => 'user',
        EngineInterface::ADMIN_ENGINE_ID => 'admin',
        EngineInterface::SBIN_ENGINE_ID => 'script',
    ];

    /**
     * @param AbstractEngine $engine
     */
    public function onAfterEngineStart($engine)
    {
        $plugin = PXMultiRegions::getInstance($this->engines[$engine->engineClass()]);
        $plugin->setSession(PXRegistry::getSession()); // prevent opening session in plugin
        $plugin->setRid();
    }
}
