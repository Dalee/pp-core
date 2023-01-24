<?php

class PXControlReset extends PXAdminWidget
{
    public function __construct(public $value)
    {
    }

    public function html()
    {
        return '<input type="reset" value="' . quot($this->value) . '" ' . $this->disable . '>';
    }
}
