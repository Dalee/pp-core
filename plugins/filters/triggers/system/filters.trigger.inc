<?php

use PP\Lib\Engine\EngineInterface;

class PXTriggerSystemFilters extends PXAbstractSystemTrigger
{

    public function onAfterEngineStart($engine)
    {
        if ($engine->engineClass() != EngineInterface::ADMIN_ENGINE_ID) {
            return;
        }

        PXPluginFilters::toggle(true);
    }
}

?>
