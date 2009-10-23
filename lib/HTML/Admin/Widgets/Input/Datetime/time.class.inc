<?php
require_once 'abstract.class.inc';
class PXInputTime extends PXInputDateTime {
	function html() {
		$null    = array(null => '--');

		$hours   = array_map(array($this, '__leadingZero'), range(0, 24));
		$hours   = $null + array_combine($hours, $hours);
		
		$sixty   = array_map(array($this, '__leadingZero'), range(0, 60));
		$seconds = $minutes = $null + array_combine($sixty, $sixty);

		$hour    = new PXInputDropDown($this->name.'[hour]',   $this->date['hour'],   $hours);
		$minute  = new PXInputDropDown($this->name.'[minute]', $this->date['minute'], $minutes);
		$second  = new PXInputDropDown($this->name.'[second]', $this->date['second'], $seconds);

		$delim   = '<span>:</span>';

		return '<span class="datetime">'.$hour->html().$delim.$minute->html().$delim.$second->html().'</span>';
	}

	function __leadingZero($val){
		return sprintf('%02d', $val);
	}
}
?>