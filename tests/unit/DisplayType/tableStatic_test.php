<?php
class TableStaticTest extends UnitTestCase {
	
	function setUp() {
		Mock::generate('PXFieldDescription');
		$this->field       = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		$this->object['test_field'] = array(array('text'));
		
		$this->tableStatic = new PXDisplayTypeTableStatic($this->field, 'tableStatic');
		
		$this->UnitTestCase();
	}
	
	function testBuildInput() {
		$html  = '<input type="hidden" name="test_field" value="Array">';
		$html .= '<table class="static"><tr><td>text</td></tr></table>';
		
		$res  = $this->tableStatic->buildInput($this->field, $this->object);
		$this->assertEqual($html,  $res);
	}
}
?>