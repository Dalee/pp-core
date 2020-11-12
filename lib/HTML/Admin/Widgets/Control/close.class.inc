<?php

class PXControlClose extends PXAdminWidget
{
    public $value;

    public function __construct($value)
    {
        $this->value = $value;
    }

    public function html()
    {
        return '<button onclick="window.close();return false">' . $this->value . '</button>';
    }
}
