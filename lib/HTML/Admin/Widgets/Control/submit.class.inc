<?php

class PXControlSubmit extends PXAdminWidget
{
    public function __construct(public $value, public $name = null, public $accessKey = null)
    {
    }

    public function html()
    {
        return sprintf(
            '<input type="submit"%s value="%s" accesskey="%s"%s>',
            !empty($this->name) ? ' name="' . $this->name . '"' : '',
            quot($this->value),
            !empty($this->accessKey) ? quot($this->accessKey) : 's',
            $this->disable
        );
    }
}
