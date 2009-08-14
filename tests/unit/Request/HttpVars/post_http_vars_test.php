<?php

class PostHttpVarsTest extends UnitTestCase {
	function setUp() {
		$this->object = new NLPostData;
	}
	
	function test_get_image_var_single_without_remove() {
		$_FILES['test_image'] = array(
			"name" => "test.jpg");

		$this->assertEqual(array("name"=>"test.jpg"), $this->object->_GetImageVar("test_image", $isArray=false));
	}

	function test_get_image_var_array() {
		# \\ for checking normalize work
		$_FILES['test_image'] = array("name"=>array("\\test.jpg", "test2.jpg"));
		$this->assertEqual(array("name"=>array("test.jpg", "test2.jpg")), $this->object->_GetImageVar("test_image", $isArray=true));
	}

	function test_get_image_var_with_remove() {
		$_FILES['test_image'] = array("name"=>array("test.jpg", "test2.jpg"));
		$this->object->raw['test_image_remove'] = array("test2.jpg"=>0);

		$this->assertEqual(array("name"=>array("test.jpg", "test2.jpg"), "remove"=>array("test2.jpg")), $this->object->_GetImageVar("test_image", $isArray=true));
	}
}


?>