<?php

class PXControlReset extends PXAdminWidget
{
    public $value;

    public function __construct($value)
    {
        $this->value = $value;
    }

    public function html()
    {
        return '<input type="reset" value="' . quot($this->value) . '" ' . $this->disable . '>';
    }
}
