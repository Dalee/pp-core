<?php

class PXControlClose extends PXAdminWidget
{
    public function __construct(public $value)
    {
    }

    public function html()
    {
        return '<button onclick="window.close();return false">' . $this->value . '</button>';
    }
}
