<?php

class CookieHttpVarsTest extends UnitTestCase {

	function setUp() {
		$_COOKIE['test1'] = serialize(array("1"));
		$_COOKIE['test2'] = myconv('k', 'w', "������ ���!");

		$this->object = new NLCookieData;
	}

	function test_set_var() {
	}

	function test_get_var() {
		$this->assertNull($this->object->_GetVar('unknown'));
		$this->assertEqual(array("1"), $this->object->_GetVar('test1'));
		$this->assertEqual('������ ���!',$this->object->_GetVar('test2'));
	}
}

?>
