<?php

class PXInputHidden extends PXAdminWidget
{
    public $value;
    public $name;

    public function __construct($name, $value)
    {
        $this->value = $value;
        $this->name = $name;
    }

    public function html()
    {
        $name = htmlspecialchars($this->name, ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET);
        $value = htmlspecialchars($this->value, ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET);

        return '<input type="hidden" name="' . $name . '" value="' . $value . '" ' . $this->disable . '>';
    }
}

