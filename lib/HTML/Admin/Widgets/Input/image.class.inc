<?php

class PXInputImage extends PXInputFile
{
    public $_default;
    public $p;
    public $width;
    public $height;

    public function __construct($name, $value, $entry = NULL)
    {
        $this->_default = 'i/v.gif';
        $this->value = $value;
        $this->name = $name;
        $this->entry = $entry;
        $this->type = 'image';
    }

    public function setparam()
    {
        $this->distension = is_null($this->entry) ? '' : '[' . $this->entry . ']';

        $w = $this->value['width'] ?? '100';
        $h = $this->value['height'] ?? '100';
        $this->p = $this->value['path'] ?? $this->_default;

        if ($w > 100 || $h > 100) {
            $this->width = ($w - $h) > 0 ? 100 : floor(100 * $w / $h);
            $this->height = ($w - $h) > 0 ? floor(100 * $h / $w) : 100;
        } else {
            $this->width = $w;
            $this->height = $h;
        }
    }

    public function preview()
    {
        return <<<HTML
			<div>
				<img src="{$this->p}" id="{$this->name}{$this->distension}src" width="{$this->width}" height="{$this->height}">
			</div>
HTML;
    }

    public function param()
    {
        $html = '<input id="' . $this->name . $this->distension . 'width"  type="text" readonly value="' . $this->value['width'] . '" >';
        $html .= '<span>ширина</span>';

        $html .= '<input id="' . $this->name . $this->distension . 'height" type="text" readonly value="' . $this->value['height'] . '">';
        $html .= '<span>высота</span>';

        $fileSize = !empty($this->value['size']) ? $this->value['size'] : ($this->value['path'] ? filesize(HTDOCS_PATH . DIRECTORY_SEPARATOR . $this->value['path']) : '');
        $html .= '<input id="' . $this->name . $this->distension . 'size"   type="text" readonly value="' . $fileSize . '">';
        $html .= '<span>размер</span>';

        return $html;

    }
}
