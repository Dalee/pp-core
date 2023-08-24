<?php

class PXInputCheckbox extends PXAdminWidget
{
    public $on;
    public $value = NULL;
    public $id = NULL;

    public function __construct(public $name, $on = false, $value = NULL)
    {
        $this->setOn($on);
        $this->setValue($value);
    }

    public function setOn($on)
    {
        $this->on = $on ? 'checked' : '';
    }

    public function setId($id)
    {
        $this->id = $id;
    }

    public function setValue($value)
    {
        $this->value = $value;
    }

    public function html()
    {
        $html = '<input type="checkbox" class="checkbox" name="' . $this->name . '" ' . $this->on;

        $html .= $this->id ? ' id="' . $this->id . '"' : '';
        $html .= !is_null($this->value) ? ' value="' . quot($this->value) . '"' : '';
        $html .= $this->disable . ' />';

        return $html;
    }
}
