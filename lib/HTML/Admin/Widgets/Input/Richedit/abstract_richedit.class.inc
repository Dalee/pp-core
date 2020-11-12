<?php

abstract class PXRichEditDecorator extends PXAdminWidget
{

    protected $editorName;

    public function initiate()
    {
        $this->parentWidget->addEditor($this->editorName);
    }
}

