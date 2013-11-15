<?php

require_once PPLIBPATH . 'HTML/Admin/Widgets/Input/Datetime/abstract.class.inc';

class PXInputDateCalendar extends PXInputDateTime {
	function html() {
		$name  = $this->name;
		$saneName = str_replace(array('[',']'),'',$name);

		$day   = is_null($this->date['day']) ? null : sprintf('%02d', $this->date['day']);
		$month = is_null($this->date['day']) ? null : sprintf('%02d', $this->date['month']);
		$year  = $this->date['year'];

		$delim = '<input type="text" class="delim" value="." ' . $this->disable . ' readonly tabindex="-1">';

		$setNullButton = $this->notNull ? '' : <<<HTML
		<button onclick="with(this.form)ctrlCalendar.calendar{$saneName}.SetNull(this.form.returnDate{$saneName}, this.form.returnDay{$saneName}, this.form.returnMonth{$saneName}, this.form.returnYear{$saneName}); return false;" {$this->disable}>обнулить</button>
HTML;
 
		$html = <<<HTML
			<span class="datetime">
				<input maxlength="2" type="text" class="first"  name="{$name}[day]"   id="returnDay{$saneName}"   value="{$day}"   {$this->disable}>{$delim}
				<input maxlength="2" type="text" class="middle" name="{$name}[month]" id="returnMonth{$saneName}" value="{$month}" {$this->disable}>{$delim}
				<input maxlength="4" type="text" class="last"   name="{$name}[year]"  id="returnYear{$saneName}"  value="{$year}"  {$this->disable}>

				<input type="hidden" id="returnDate{$saneName}" value="{$day}.{$month}.{$year}">

				<iframe src="tools/calendar/index.html" name="calendar{$saneName}" id="calendar{$saneName}" scrolling="no" frameborder="0"></iframe>

				<button title="календарь" class="calendar" onfocus="this.blur();" onclick="with(this.form)ctrlCalendar.calendar{$saneName}.DoModal(this.form.returnDate{$saneName}, this.form.returnDay{$saneName}, this.form.returnMonth{$saneName}, this.form.returnYear{$saneName}); return false;" {$this->disable}></button>

				{$setNullButton}
			</span>
HTML;

		return preg_replace('/>\s+</'.REGEX_MOD, '><', $html);
	}
}
