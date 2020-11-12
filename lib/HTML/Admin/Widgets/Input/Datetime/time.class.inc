<?php

require_once PPLIBPATH . 'HTML/Admin/Widgets/Input/Datetime/abstract.class.inc';

class PXInputTime extends PXInputDateTime
{
    public function html()
    {
        $null = [null => '--'];

        $hours = array_map([$this, '__leadingZero'], range(0, 23));
        $hours = $null + array_combine($hours, $hours);

        $sixty = array_map([$this, '__leadingZero'], range(0, 59));
        $seconds = $minutes = $null + array_combine($sixty, $sixty);

        $hour = new PXInputDropDown($this->name . '[hour]', $this->__leadingZero($this->date['hour']), $hours);
        $minute = new PXInputDropDown($this->name . '[minute]', $this->__leadingZero($this->date['minute']), $minutes);
        $second = new PXInputDropDown($this->name . '[second]', $this->__leadingZero($this->date['second']), $seconds);

        $delim = '<span>:</span>';

        return '<span class="datetime">' . $hour->html() . $delim . $minute->html() . $delim . $second->html() . '</span>';
    }

    public function __leadingZero($val)
    {
        return sprintf('%02d', $val);
    }
}
