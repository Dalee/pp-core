<?php
require_once 'abstract.class.inc';
class PXInputTime extends PXInputDateTime {
	function html() {
		$hours = $minutes = $seconds = array(null => '--');

		for ($i=0; $i<24;$i++) {
			$hours[$i] = sprintf('%02d', $i);
		}

		for ($i=0; $i<60;$i++) {
			$seconds[$i] = $minutes[$i] = sprintf('%02d', $i);
		}

		$hour   = new PXInputDropDown($this->name.'[hour]',   $this->date['hour'],   $hours);
		$minute = new PXInputDropDown($this->name.'[minute]', $this->date['minute'], $minutes);
		$second = new PXInputDropDown($this->name.'[second]', $this->date['second'], $seconds);

		$delim = '<span>:</span>';

		return '<span class="datetime">'.$hour->html().$delim.$minute->html().$delim.$second->html().'</span>';
	}
}
?>