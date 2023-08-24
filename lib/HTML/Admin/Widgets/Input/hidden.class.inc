<?php

class PXInputHidden extends PXAdminWidget
{
    public function __construct(public $name, public $value)
    {
    }

    public function html()
    {
        $name = htmlspecialchars((string) $this->name, ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET);
        $value = htmlspecialchars((string) $this->value, ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET);

        return '<input type="hidden" name="' . $name . '" value="' . $value . '" ' . $this->disable . '>';
    }
}

