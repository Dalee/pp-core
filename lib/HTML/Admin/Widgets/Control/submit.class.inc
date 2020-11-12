<?php

class PXControlSubmit extends PXAdminWidget
{
    public $value;
    public $name = null;
    public $accessKey = null;

    public function __construct($value, $name = null, $accessKey = null)
    {
        $this->value = $value;
        $this->name = $name;
        $this->accessKey = $accessKey;
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
