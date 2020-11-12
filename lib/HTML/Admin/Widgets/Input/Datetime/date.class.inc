<?php

require_once PPLIBPATH . 'HTML/Admin/Widgets/Input/Datetime/abstract.class.inc';

class PXInputDate extends PXInputDateTime
{
    public function html()
    {
        $years = $days = [null => '--'];
        for ($i = 1; $i <= 31; $i++) {
            $days[$i] = sprintf("%02d", $i);
        }

        $monthes = [
            '--',
            'января',
            'февраля',
            'марта',
            'апреля',
            'мая',
            'июня',
            'июля',
            'августа',
            'сентября',
            'октября',
            'ноября',
            'декабря',
        ];

        for ($i = date("Y") + 1; $i >= 1970; $i--) {
            $years[$i] = $i;
        }

        $delim = '<span>/</span>';

        $year = new PXInputDropDown($this->name . '[year]', (int)$this->date['year'], $years);
        $month = new PXInputDropDown($this->name . '[month]', (int)$this->date['month'], $monthes);
        $day = new PXInputDropDown($this->name . '[day]', (int)$this->date['day'], $days);

        return $day->html() . $delim . $month->html() . $delim . $year->html();
    }
}
