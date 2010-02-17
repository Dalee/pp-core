<?php
class CheckBoxTest extends UnitTestCase {
	
	function setUp() {
		Mock::generate('PXFieldDescription');
		$this->field   = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		
		$this->checkbox = new PXDisplayTypeCheckbox($this->field, 'checkbox');
		
		$this->UnitTestCase();
	}
	
	function testGetFromRequest() {
		$request = new PXRequest();
		$var     = $request->getData;
		
		$var->raw['test_field'] = true;
		$request->postData = & $var;
		$param['request'] = $request;
		
		$object = array();
		
		$tt = $this->checkbox->getFromRequest($this->field, $object, $param);
		$this->assertEqual($tt, true);
	}
	
	function testBuildCell() {
		$resOn = '<div class="truecheck" title="да"></div>';
		$resOff = '';
		
		$object = array('test_field' => true);
		$tt = $this->checkbox->buildCell($this->field, $object);
		$this->assertEqual($tt, $resOn);
		
		$object['test_field'] = false;
		$tt = $this->checkbox->buildCell($this->field, $object);
		$this->assertEqual($tt, $resOff);
	}
	
	function testBuildInput() {
		$resOff = '<input type="checkbox" class="checkbox" name="test_field"  />';
		$resOn  = '<input type="checkbox" class="checkbox" name="test_field" checked />';
		
		$object = array('test_field' => true);
		$tt = $this->checkbox->buildInput($this->field, $object);
		$this->assertEqual($tt, $resOn);
		
		$object['test_field'] = false;
		$tt = $this->checkbox->buildInput($this->field, $object);
		$this->assertEqual($tt, $resOff);
	}
}
?>
