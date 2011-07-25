<?php
require_once 'abstract.class.inc';
class PXInputDateHidden extends PXInputDateTime {
	function html() {
		$values = array(
			'day'   => sprintf("%02d", $this->date['day']),
			'month' => sprintf("%02d", $this->date['month']),
			'year'  => $this->date['year']
		);

		$html = '';
		foreach($values as $k=>$v) {
			$html .= '<input type="hidden" name="'.$this->name.'['.$k.']"   value="'.htmlspecialchars($v).'">';
		}

		return $html;
	}
}
?>