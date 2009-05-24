<?php
class DateTest extends UnitTestCase {
	
	function setUp() {
		Mock::generate('PXFieldDescription');
		$this->field       = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		
		$this->object['test_field'] = '1-1-2009 9:00:00';
		
		$this->date = new PXDisplayTypeDate($this->field, 'date');
		
		$this->UnitTestCase();
	}
	
	function testBuildInput() {
		$html = '<div class="datetime">';
		$html .= '<input type="hidden" name="test_field[hour]"   value="09">';
		$html .= '<input type="hidden" name="test_field[minute]" value="00">';
		$html .= '<input type="hidden" name="test_field[second]" value="00">';
		$html .= '<input maxlength="2" type="text" class="first" name="test_field[day]"   id="returnDaytest_field"   value="01">';
		$html .= '<input type="text" class="delim" value="." readonly tabindex="-1">';
		$html .= '<input maxlength="2" type="text" class="middle" name="test_field[month]" id="returnMonthtest_field" value="01">';
		$html .= '<input type="text" class="delim" value="." readonly tabindex="-1">';
		$html .= '<input maxlength="4" type="text" class="last" name="test_field[year]"  id="returnYeartest_field"  value="2009">';
		$html .= '<input type="hidden" id="returnDatetest_field" value="">';
		$html .= '<iframe src="tools/calendar/index.html" name="calendartest_field" id="calendartest_field" scrolling="no" frameborder="0"></iframe>';
		$html .= '<button title="календарь" class="calendar" onFocus="this.blur();"
				  onclick="with(this.form)ctrlCalendar.calendartest_field.DoModal(this.form.returnDatetest_field, this.form.returnDaytest_field, this.form.returnMonthtest_field, this.form.returnYeartest_field);return false;"></button>';
		$html .= '<button onclick="with(this.form)ctrlCalendar.calendartest_field.SetNull(this.form.returnDatetest_field, this.form.returnDaytest_field, this.form.returnMonthtest_field, this.form.returnYeartest_field);return false;">обнулить</button>';
		$html .= '</div>';
		$res  = $this->date->buildInput($this->field, $this->object);
		$this->assertEqual($html,  $res);
	}
}
?>