<?php

class BaseHttpVarsTest extends UnitTestCase {
	function setUp() {
		$this->object = new NLHTTPVarsData;
	}	

	function test_charchek_machine() {
		$this->object->raw['charcheck'] = 'йцукен';
		$this->assertEqual('k', $this->object->charcheck_machine($this->object->raw['charcheck']));

		$this->object->raw['charcheck'] = myconv('k', 'w', 'йцукен');
		$this->assertEqual('w', $this->object->charcheck_machine($this->object->raw['charcheck']));

		$this->object->raw['charcheck'] = myconv('k', 'u', 'йцукен');
		$this->assertEqual('u', $this->object->charcheck_machine($this->object->raw['charcheck']));
	}	

	function test_is_utf() {
		$this->object->raw['test'] = myconv('k', 'u', 'йцукен');
		$this->assertTrue($this->object->isUtf());
	}

	function test_normalize_data() {
	}

	function check_normalizing($to) {
		$this->object->raw['test'] = myconv('k', $to, 'Привет Мир!');
		$this->object->_NormalizeData($this->object->raw);
		$expected = "Привет Мир!";
	
		$this->assertEqual($expected, $this->object->raw['test']);
	}

	function test_normalize() {
		$this->check_normalizing("w");
		$this->check_normalizing("u");
	}

	function test_get_var() {
		$this->object->raw['test'] = 1;
		$expected = 1;
		$actual = $this->object->_GetVar('test');

		$this->assertEqual($expected, $actual);
	}
	
	function test_is_set_var() {
		$this->object->raw['test'] = 1;
		$this->assertTrue($this->object->_isSetVar('test'));
		$this->assertFalse($this->object->_isSetVar('unknown'));
	}

	function test_get_numeric_var() {
		$this->object->raw['test'] = '1';
		$this->object->raw['test2'] = '1a';

		$this->assertEqual(1, $this->object->_GetNumericVar('test'));
		$this->assertNull($this->object->_GetNumericVar('test2'));
	}

	function test_get_array_var_without_remove_empty_rows() {
		$testable = array("test00"=>"", "test01"=>array("test10"=>"", "test11"=>array("testf1"=>"", "testf2"=>"hello world!", "testf3"=>"bla-bla")));
		$this->object->raw['test'] = $testable;
		$actual = $this->object->_GetArrayVar('test', $killEmptyRows = false);

		$expected = $testable;

		$this->assertEqual($expected, $actual);
	}
	
	function test_get_array_var_with_remove_empty_rows() {
		$testable = array("test00"=>"", "test01"=>array("test10"=>"", "test11"=>array("testf1"=>"", "testf2"=>"hello world!", "testf3"=>"bla-bla")));
		$this->object->raw['test'] = $testable;
		$actual = $this->object->_GetArrayVar('test', $killEmptyRows = true);

		$expected = array("test01"=>array("test11"=>array("testf2"=>"hello world!", "testf3"=>"bla-bla")));
		$this->assertEqual($expected, $actual);
	}

	function test_get_checkbox_var() {
		$this->object->raw['test'] = 1;
		$this->assertTrue($this->object->_GetCheckBoxVar('test'));
		$this->assertFalse($this->object->_GetCheckBoxVar('unknown'));
	}

	function test_get_time_stamp_var() {
		$exp1 = date("d-n-Y G:i:s");
		$this->assertEqual($exp1, $this->object->_GetTimeStampVar('unknown'));

		$this->object->raw['test'] = array('year'=>'', 'month'=>'', 'day'=>'');
		$this->assertNull($this->object->_GetTimeStampVar('test'));

		// ORLY??
		$this->object->raw['test'] = array('year'=>'2009', 'month'=>'275', 'day'=>'12340', 'hour'=>'1', 'minute'=>'123', 'second'=>'87');
		$this->assertEqual("12340.275.2009 01:123:87", $this->object->_GetTimeStampVar('test'));
	}

	function test_get_date_var() {
		$exp1 = date("j-n-Y");
		$this->assertEqual($exp1, $this->object->_GetDateVar('unknown'));

		$this->object->raw['test'] = array('year' => '', 'month' => '', 'day' => '');
		$this->assertNull($this->object->_GetDateVar('test'));
		
		$this->object->raw['test'] = array('year' => '30100', 'month' => '11', 'day' => '11');
		$this->assertEqual('11.11.30100', $this->object->_GetDateVar('test'));
	}

	function test_get_ip_addr_var() {
	}

	function test_is_set() {
	}

	function test_get_all() {
	}
}

?>
