<?php

use PP\Plugin\AbstractPlugin;

class PXPluginSortableAcl extends AbstractPlugin
{
    public function initialize($app)
    {
        $this->loadModule('sortableacl');
    }
}

