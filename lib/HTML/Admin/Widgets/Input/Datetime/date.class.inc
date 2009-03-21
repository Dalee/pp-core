<?php
require_once 'abstract.class.inc';
class PXInputDate extends PXInputDateTime {
	function html() {
		$years = $days = array(null => '--');
		for ($i=1; $i<=31; $i++) {
			$days[$i] = sprintf("%02d", $i);
		}

		$monthes = array(
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
			'декабря'
		);

		for ($i=date("Y")+1; $i >= 1970; $i--) {
			$years[$i] = $i;
		}

		$delim= '<span>/</span>';

		$year  = new PXInputDropDown($this->name.'[year]',  $this->date['year'],  $years);
		$month = new PXInputDropDown($this->name.'[month]', $this->date['month'], $monthes);
		$day   = new PXInputDropDown($this->name.'[day]',   $this->date['day'],   $days);


		return $day->html().$delim.$month->html().$delim.$year->html();
	}
}
?>