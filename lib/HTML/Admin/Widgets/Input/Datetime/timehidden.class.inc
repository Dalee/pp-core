<?php
require_once 'abstract.class.inc';
class PXInputTimeHidden extends PXInputDateTime {
	function html() {
		$html = '';

		foreach(array('hour', 'minute', 'second') as $datePart) {
			$html .= '<input type="hidden" name="'.$this->name.'['.$datePart.']" value="'.sprintf('%02d', $this->date[$datePart]) .'">';
		}

		return $html;
	}
}
?>