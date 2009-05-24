<?php
class ImageTest extends UnitTestCase {
	
	function setUp() {
		Mock::generate('PXFieldDescription');
		$this->field       = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		
		$this->object['test_field'] = array('width'=>null, 'height'=>null, 'path'=>null);
		
		$this->image = new PXDisplayTypeImage($this->field, 'image');
		
		$this->UnitTestCase();
	}
	
	function testBuildInput() {
		$html  = '<div class="imagepreview">';
		$html .= '<div>';
		$html .= '<img src="i/v.gif" id="test_fieldsrc" width="100" height="100">';
		$html .= '</div>';
		$html .= '<input class="file" type="file" id="test_field" name="test_field" onpropertychange="Preview(this.value, \'image\', \'test_field\');"><br><input id="test_fieldwidth"  type="text" readonly value="" ><span>ширина</span><input id="test_fieldheight" type="text" readonly value=""><span>высота</span><input id="test_fieldsize"   type="text" readonly value=""><span>размер</span></div>';
		
		$res  = $this->image->buildInput($this->field, $this->object);
		$this->assertEqual($html,  $res);
	}
	
	function testBuildCell() {
		$html = '<div class="imagepreview image-in-cell"><div class="small"></div></div>';
		$res  = $this->image->buildCell($this->field, $this->object);
		$this->assertEqual($html,  $res);
	}
}
?>