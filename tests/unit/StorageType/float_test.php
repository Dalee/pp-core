<?php
class FloatTypeTest extends UnitTestCase {
	
	function setUp() {
		$this->UnitTestCase();
		
		Mock::generate('PXFieldDescription');
		
		$this->field  = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		
		$this->object['test_field'] = '12,456ettt';
		
		$this->float = new PXStorageTypeFloat($field, 'float');
	}
	
	function testGetFromRequest() {
		$res = $this->float->getFromRequest($this->field, $this->object, $param);
		$this->assertIsA($res, 'float');
	}
	
	function testNormalizeObjectAttribute() {
		Mock::generate('NLDBDescription');
		$dbDescription = new MockNLDBDescription();
		
		$param['db']       = new NLPGSQLDatabase($dbDescription);
		$param['dbFields'] = array();
		$param['dbValues'] = array();
		
		$test = 12.456;
		$res = $this->float->normalizeObjectAttribute($this->field, $this->object, $param);
		$this->assertEqual($res[0], $test);
	}
}
?>