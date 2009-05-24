<?php
class FileTest extends UnitTestCase {
	
	function setUp() {
		Mock::generate('PXFieldDescription');
		$this->field       = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		
		$this->object['test_field'] = array('filename' => 'fname',
											'size'     => 'fsize',
											'type'     => 'ftype',
											'fullpath' => 'fullpath');
		
		$this->file = new PXDisplayTypeFile($this->field, 'file');
		
		$this->UnitTestCase();
	}
	
	function testBuildInput() {
		$html  = '<div class="imagepreview"><input class="file" type="file" id="test_field" name="test_field" onpropertychange="Preview(this.value, \'file\', \'test_field\');">';
		$html .= '<br>';
		$html .= '<input type="text" readonly value="fname" ><span>���</span>';
		$html .= '<input type="text" readonly value="fsize" ><span>������</span>';
		$html .= '<input type="text" readonly value="ftype" ><span>���</span>';
		$html .= '<br>';
		$html .= '<a href="fullpath" target="_blank" class="img-preview">��������</a> ';
		$html .= '<button onclick="return ToClipboardMulti(\'fullpath\', 0, 0, \'file\')">HTML � �����</button>';
		$html .= '<br>';
		$html .= '<input class="checkbox" type="checkbox" id="test_field_remove" name="test_field_remove[ullpath]">';
		$html .= '<label for="test_field_remove">������� ����</label>';
		$html .= '</div>';
		
		$res  = $this->file->buildInput($this->field, $this->object);
		$this->assertEqual($html,  $res);
	}
	
	function testBuildCell() {
		$html = '<a href="fullpath" class="file fname">fname</a> 0��</br>';
		$res  = $this->file->buildCell($this->field, $this->object);
		$this->assertEqual($html,  $res);
	}
}
?>