<?php
class StaticTest extends UnitTestCase {
	
	function setUp() {
		Mock::generate('PXFieldDescription');
		$this->field       = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		
		Mock::generate('PXDirectoryDescription');
		
		$this->object = array('test_field' => 'static_value');
		
		$this->static = new PXDisplayTypeStatic($this->field, 'static');
		
		$this->UnitTestCase();
	}
	
	function testBuildInput() {
		$html = '<input type="hidden" name="test_field" value="static_value" >static_value';
		$res  = $this->static->buildInput($this->field, $this->object);
		$this->assertEqual($res,  $html);
		
		$directory     = new MockPXDirectoryDescription();
		$directory->setReturnValue('GetList', array('static_value'=>'123'));
		$this->field->values = $directory;
		
		$html = '<input type="hidden" name="test_field" value="static_value" >123';
		$res  = $this->static->buildInput($this->field, $this->object);
		$this->assertEqual($res,  $html);
	}
	
	function testBuildCell() {
		$res  = $this->static->buildCell($this->field, $this->object);
		$this->assertEqual($res, 'static_value');
		
		$directory     = new MockPXDirectoryDescription();
		$directory->values = array('static_value' => array('title' => 'title'));
		$this->field->values = $directory;
		
		$res  = $this->static->buildCell($this->field, $this->object);
		$this->assertEqual($res, 'title');
	}
}
?>
