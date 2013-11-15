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
        /*
		    '<div class="datetime">';
            '<input type="hidden" name="test_field[hour]"   value="09">';
            '<input type="hidden" name="test_field[minute]" value="00">';
            '<input type="hidden" name="test_field[second]" value="00">';
            '<input maxlength="2" type="text" class="first" name="test_field[day]"   id="returnDaytest_field"   value="01">';
            '<input type="text" class="delim" value="." readonly tabindex="-1">';
            '<input maxlength="2" type="text" class="middle" name="test_field[month]" id="returnMonthtest_field" value="01">';
            '<input type="text" class="delim" value="." readonly tabindex="-1">';
            '<input maxlength="4" type="text" class="last" name="test_field[year]"  id="returnYeartest_field"  value="2009">';
            '<input type="hidden" id="returnDatetest_field" value="">';
            '<iframe src="tools/calendar/index.html" name="calendartest_field" id="calendartest_field" scrolling="no" frameborder="0"></iframe>';
            '<button title="календарь" class="calendar" onFocus="this.blur();"
             onclick="with(this.form)ctrlCalendar.calendartest_field.DoModal(this.form.returnDatetest_field, this.form.returnDaytest_field, this.form.returnMonthtest_field, this.form.returnYeartest_field);return false;"></button>';
            '<button onclick="with(this.form)ctrlCalendar.calendartest_field.SetNull(this.form.returnDatetest_field, this.form.returnDaytest_field, this.form.returnMonthtest_field, this.form.returnYeartest_field);return false;">обнулить</button>';
            '</div>';
         */

		$res  = $this->date->buildInput($this->field, $this->object);
        
        // there is no <DIV.+</DIV>???
        //enclosed with <div..>...</div>
        //$this->assertPattern('#^<div[^>]+>.+</div>$#sm',  $res);
        
        $this->assertPattern('/input type="hidden" name="test_field\[hour\]" value="09"/m',  $res);
		$this->assertPattern('/input type="hidden" name="test_field\[minute\]" value="00"/m',  $res);
		$this->assertPattern('/input type="hidden" name="test_field\[second\]" value="00"/m',  $res);

		$this->assertPattern('/input maxlength="2" type="text".+name="test_field\[day\]"\s+id="returnDaytest_field"\s+value="01"/m',  $res);
		$this->assertPattern('/input maxlength="2" type="text".+name="test_field\[month\]"\s+id="returnMonthtest_field"\s+value="01"/m',  $res);
		$this->assertPattern('/input maxlength="4" type="text".+name="test_field\[year\]"\s+id="returnYeartest_field"\s+value="2009"/m',  $res);
        
	}
}
?>
