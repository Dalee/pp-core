<?php
class BooleanTypeTest extends UnitTestCase {
	
	function setUp() {
		$this->UnitTestCase();
		
		Mock::generate('PXFieldDescription');
		
		$this->field  = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		
		$this->bool = new PXStorageTypeBoolean($this->field, 'bool');
	}
	
	function testGetFromRequest() {
		$object['test_field'] = '';
		$res = $this->bool->getFromRequest($this->field, $object, $param);
		$this->assertIsA($res, 'bool');
	}
	
	function testNormalize() {
		Mock::generate('NLDBDescription');
		$dbDescription = new MockNLDBDescription();
		
		$param['db'] = new NLPGSQLDatabase($dbDescription);
		$object['test_field'] = '1';
		$res = $this->bool->normalize($this->field, $object, $param);
		$this->assertTrue($res);
		
		$object['test_field'] = 't';
		$res = $this->bool->normalize($this->field, $object, $param);
		$this->assertTrue($res);
		
		$object['test_field'] = 'string';
		$res = $this->bool->normalize($this->field, $object, $param);
		$this->assertFalse($res);
	}
}
?>