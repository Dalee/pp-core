<?php
class DropdownTest extends UnitTestCase {
	
	function setUp() {
		Mock::generate('PXDirectoryDescription');
		Mock::generate('PXFieldDescription');
		$this->field       = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		
		$this->object['test_field'] = 'dropdown_object';
		
		$directory     = new MockPXDirectoryDescription();
		$directory->setReturnValue('GetList', array('dropdown_object'=>'123'));
		$this->field->values = $directory;
		
		$this->dropdown = new PXDisplayTypeDropdown($this->field, 'dropdown');
		
		$this->UnitTestCase();
	}
	
	function testBuildInput() {
		$html = '<select name="test_field"><option value="dropdown_object" selected>123</option></select>';
		$res  = $this->dropdown->buildInput($this->field, $this->object);
		$this->assertEqual($res,  $html);
	}
	
	function testBuildCell() {
		$this->field->values->displayField = 'test_field';
		$this->field->values->values['dropdown_object']['test_field'] = 'test';
		
		$res  = $this->dropdown->buildCell($this->field, $this->object);
		$this->assertEqual($res,  'test');
	}
}
?>