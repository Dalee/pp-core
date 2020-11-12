<?php

use PP\Plugin\AbstractPlugin;

class PXPluginEvents extends AbstractPlugin
{
    public function initialize($app)
    {
        $this->load('lib/events_queue.class.inc');
        $this->loadCronrun('events');
    }
}
