<?php

use PP\Plugin\AbstractPlugin;

class PXPluginZoom extends AbstractPlugin
{
    public function initialize($app)
    {
        $this->load("lib/zoom_filter.class.inc");
        $this->loadTrigger("system/zoom");
    }
}
