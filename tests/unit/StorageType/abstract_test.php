<?php
class AbstractTest extends UnitTestCase {
	
	function setUp() {
		$this->UnitTestCase();
		
		Mock::generate('PXFieldDescription');
		
		$this->field  = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		$this->field->description = '��������';
		
		$this->object['test_field'] = '';
		
		$this->abstract = new PXStorageTypeBoolean($this->field, '');
	}
	
	function testObjectsSortOrderString() {
		Mock::generate('PXAdminTable');
		$table = new MockPXAdminTable();
		$table->setReturnValue('_BuildHref', 'url');
		
		$param['table'] = $table;
		$param['dtype'] = 'dtype';
		
		$param['order'] = 'test_field DESC';
		$html = '<a  class="down" href="url" title="����������� �� ���� &bdquo;��������&ldquo;">��������</a>';
		
		$res  = $this->abstract->objectsSortOrderString($this->field, $this->object, $param);
		$this->assertEqual($html,  $res);
		
		$param['order'] = 'test_field';
		$html = '<a  class="up" href="url" title="����������� �� ���� &bdquo;��������&ldquo; � �������� �������">��������</a>';
		
		$res  = $this->abstract->objectsSortOrderString($this->field, $this->object, $param);
		$this->assertEqual($html,  $res);
	}
	
	function testNormalizeObjectAttributeString() {
		$res  = $this->abstract->normalizeObjectAttributeString($this->field, $this->object, $param);
		$this->assertIsA($res, 'array');
	}
}
?>