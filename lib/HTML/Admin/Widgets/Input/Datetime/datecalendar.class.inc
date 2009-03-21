<?php
require_once 'abstract.class.inc';
class PXInputDateCalendar extends PXInputDateTime {
	function html() {
		$name  = $this->name;

		$day   = is_null($this->date['day']) ? null : sprintf('%02d', $this->date['day']);
		$month = is_null($this->date['day']) ? null : sprintf('%02d', $this->date['month']);
		$year  = $this->date['year'];

		$delim = '<input type="text" class="delim" value="." readonly tabindex="-1">';

		$html = <<<HTML
			<span class="datetime">
				<input maxlength="2" type="text" class="first"  name="{$name}[day]"   id="returnDay{$name}"   value="{$day}"   {$this->disable}>{$delim}
				<input maxlength="2" type="text" class="middle" name="{$name}[month]" id="returnMonth{$name}" value="{$month}" {$this->disable}>{$delim}
				<input maxlength="4" type="text" class="last"   name="{$name}[year]"  id="returnYear{$name}"  value="{$year}"  {$this->disable}>

				<input type="hidden" id="returnDate{$name}" value="">

				<iframe src="tools/calendar/index.html" name="calendar{$name}" id="calendar{$name}" scrolling="no" frameborder="0"></iframe>

				<button title="календарь" class="calendar" onfocus="this.blur();" onclick="with(this.form)ctrlCalendar.calendar{$name}.DoModal(this.form.returnDate{$name}, this.form.returnDay{$name}, this.form.returnMonth{$name}, this.form.returnYear{$name}); return false;" {$this->disable}></button>

				<button onclick="with(this.form)ctrlCalendar.calendar{$name}.SetNull(this.form.returnDate{$name}, this.form.returnDay{$name}, this.form.returnMonth{$name}, this.form.returnYear{$name}); return false;" {$this->disable}>обнулить</button>
			</span>
HTML;

		return preg_replace('/>\s+</', '><', $html);
	}
}
?>