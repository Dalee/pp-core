<?php

class PXInputTextarea extends PXAdminWidget
{
    public function __construct(public $name, public $value, public $height, public $maxlength = null, public $wrap = "soft")
    {
    }

    public function html()
    {
        $height = strpos((string) $this->height, '%') ? $this->height : $this->height . 'px';
        $max = isset($this->maxlength) ? 'maxlength="' . (int)$this->maxlength . '"' : '';

        return sprintf(
            '<textarea name="%s" wrap="%s" style="height: %s;" %s %s>%s</textarea>',
            $this->name,
            $this->wrap,
            $height,
            $this->disable,
            $max,
            "\n" . htmlspecialchars((string) $this->value, ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET)
        );
    }
}
