<?php
class FileTypeTest extends UnitTestCase {
	
	function setUp() {
		$this->UnitTestCase();
		$this->aiTestPath = BASEPATH.'/site/htdocs/ai/test/';
		
		Mock::generate('PXFieldDescription');
		
		$this->field  = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		
		$this->object['test_field'] = array('name' => 'fname',
											'size'     => 'fsize',
											'type'     => 'txt',
											'fullpath' => 'fullpath',
											'tmp_name' => $this->aiTestPath.'tmp_name.txt');
		$this->object['id'] = '1';
		
		$this->param['format']         = 'test';
		$this->param['id']             = '1';
		$this->param['allowed']['txt'] = 'txt'; 
		
		$this->file = new PXStorageTypeFile($this->field, 'file');
	}
		
	function testProceedFile() {
		// создадим временный файл для теста
		$testTmp = $this->aiTestPath.'tmp_name.txt';
		
		if (!file_exists($testTmp)) {
			MakeDirIfNotExists($this->aiTestPath);
			fopen($this->aiTestPath.'tmp_name.txt', 'w');
		}
		
		$this->file->proceedFile($this->field, $this->object, $this->param);
		
		$res = file_exists($this->aiTestPath.'1/test_field/fname.txt');
		$this->assertTrue($res);
	}
	
	function testNormalize() {
		$test = array(
			'filename' => 'fname.txt',
			'fullpath' => '/ai/test/1/test_field/fname.txt',
			'type'     => mime_content_type($this->aiTestPath.'1/test_field/fname.txt'),
			'size'     => filesize($this->aiTestPath.'1/test_field/fname.txt')
		);
		$res = $this->file->normalize($this->field, $this->object, $this->param);
		$this->assertEqual($res['size'], $test['fullpath']);
	}
}
?>