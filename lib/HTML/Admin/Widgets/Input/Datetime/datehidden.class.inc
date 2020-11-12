<?php

require_once PPLIBPATH . 'HTML/Admin/Widgets/Input/Datetime/abstract.class.inc';

class PXInputDateHidden extends PXInputDateTime
{
    public function html()
    {
        $values = [
            'day' => sprintf("%02d", $this->date['day']),
            'month' => sprintf("%02d", $this->date['month']),
            'year' => $this->date['year'],
        ];

        $html = '';
        foreach ($values as $k => $v) {
            $html .= '<input type="hidden" name="' . $this->name . '[' . $k . ']"   value="' . quot($v) . '">';
        }

        return $html;
    }
}
