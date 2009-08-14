<?php

class UserRequestTest extends UnitTestCase {

	function setUp() {
		$_POST['area'] = 'test_place';
		$_POST['sid'] = 1;
		$_POST['cid'] = 1;
		$_POST['id'] = 1;
		$_POST['format'] = 'some';
		$_POST['parent'] = 1;
		$_POST['action'] = 'some';

		$_SERVER['REQUEST_METHOD'] = "POST";

		$_COOKIE['leafStatus'] = array("some"=>"close", "another"=>"open", "some another"=>"open");
		$this->object = new PXRequestUser;
	}


	//сие множество тупых гет-тестов создано  для проверки того что данные методы в качестве 
	//переменной из реквеста получают именно те именна что должны быть	
	function test_get_area() {
		$this->assertEqual("test_place", $this->object->getArea());
	}

	function test_get_leaf_status() {
		//substr 6 why?
		$this->assertEqual(array("r", "nother"), $this->object->getLeafStatus());
	}

	function test_get_sid() {
		$this->assertEqual(1, $this->object->getSid());
	}
	
	function test_get_cid() {
		$this->assertEqual(1, $this->object->getCid());
	}
	
	function test_get_id() {
		$this->assertEqual(1, $this->object->getId());
	}
	
	function test_get_format() {
		$this->assertEqual('some', $this->object->getFormat());
	}
	
	function test_get_parent() {
		$this->assertEqual(1, $this->object->getParent());
	}
	
	function test_get_action() {
		$this->assertEqual('some', $this->object->getAction());
	}

	#studing reference format ...
	function test_get_links() {
	}

	function test_get_content_object() {
	}
}

?>
