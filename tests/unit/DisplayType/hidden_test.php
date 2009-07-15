<?php
class HiddenTest extends UnitTestCase {
	
	function setUp() {
		Mock::generate('PXFieldDescription');
		$this->field       = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		
		$this->object = array('test_field' => 'hidden_value');
		
		$this->hidden = new PXDisplayTypeHidden($this->field, 'hidden');
		
		$this->UnitTestCase();
	}
	
	function testBuildRow() {
		$html = '<input type="hidden" name="test_field" value="hidden_value" >';
		$res  = $this->hidden->buildRow($this->field, $this->object);
		$this->assertEqual($res,  $html);
	}
}
?>