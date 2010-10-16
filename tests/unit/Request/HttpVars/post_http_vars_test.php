<?php

class PostHttpVarsTest extends UnitTestCase {
	private $image_var;
	private $images_array_var;
	private $image_to_remove_var;

	function setUp() {
		$this->image_var = array(
			"name"  => "1px.gif",
			"error" => 0,
			"size"  => 43
		);

		$this->images_array_var = array(
			"name"  => array("test.jpg", "test2.png"),
			"error" => array(0, 0),
			"size"  => array(2048, 4096)
		);

		$this->image_to_remove_var = array(
			"test.jpg"
		);

		$_FILES['image']                = $this->image_var;
		$_FILES['images_array']         = $this->images_array_var;
		$_FILES['images_array_r']       = $this->images_array_var;
		$_POST['images_array_r_remove'] = $this->image_to_remove_var;

		$this->object = new NLPostData();
	}
	
	function test_get_image_var_single_without_remove() {
		$this->assertEqual($this->image_var, $this->object->_GetImageVar('image', false));
	}

	function test_get_image_var_array() {
		$this->assertEqual($this->images_array_var, $this->object->_GetImageVar('images_array', true));
	}

	function test_get_image_var_with_remove() {
		$this->assertEqual(
			array_merge($this->images_array_var, array('remove' => $this->image_to_remove_var)),
			$this->object->_GetImageVar('images_array_r', true)
		);
	}

}


?>