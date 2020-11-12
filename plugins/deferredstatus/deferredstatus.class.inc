<?php

use PP\Plugin\AbstractPlugin;

class PXPluginDeferredStatus extends AbstractPlugin
{

    public function initialize($app)
    {
        $this->load('lib/deferredstatus_helper.class.inc');
        PXDeferredStatusHelper::init();

        $this->loadTrigger('database/deferredstatus');
        $this->loadCronrun('deferredstatus');
        $this->loadDisplayType('sysdeferredlist');
    }

}
