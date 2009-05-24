<?php
class SerializedTypeTest extends UnitTestCase {
	
	function setUp() {
		$this->UnitTestCase();
		
		Mock::generate('PXFieldDescription');
		
		$this->field  = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		
		$this->object['test_field'] = array('data1', 'data2');
		
		$this->serialized = new PXStorageTypeSerialized($field, 'serialized');
	}
	
	function testNormalisation() {
		$param['dbFields'] = array();
		$param['dbValues'] = array();
		
		$object1 = $this->object['test_field'];
		$res = $this->serialized->normalizeObjectAttribute($this->field, $this->object, $param);
		
		$this->object['test_field'] = $res[0];
		
		$res = $this->serialized->normalize($this->field, $this->object, $param);
		$object2 = $res;
		
		$this->assertEqual($object1, $object2);
	}
}
?>