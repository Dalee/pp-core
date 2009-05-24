<?php
class FlashTest extends UnitTestCase {
	
	function setUp() {
		Mock::generate('PXFieldDescription');
		$this->field       = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		
		$this->object['test_field'] = array('width'=>null, 'height'=>null, 'path'=>null);
		$this->object['id'] = 'id';
		
		$this->flash = new PXDisplayTypeFlash($this->flash, 'image');
		
		$this->UnitTestCase();
	}
	
	function testBuildInput() {
		$html = '<div class="imagepreview">';
		$html .= '<div><script type="text/javascript">ShowFlash(\'i/l.swf?path=i/v.swf\', 100, 100, \'test_fieldsrc\');</script></div>';
		$html .= '<input class="file" type="file" id="test_field" name="test_field" onpropertychange="Preview(this.value, \'flash\', \'test_field\');">';
		$html .= '<br>';
		$html .= '<input id="test_fieldwidth"  type="text" readonly value="" ><span>ширина</span>';
		$html .= '<input id="test_fieldheight" type="text" readonly value=""><span>высота</span>';
		$html .= '<input id="test_fieldsize"   type="text" readonly value=""><span>размер</span>';
		$html .= '</div>';
		
		$res  = $this->flash->buildInput($this->field, $this->object);
		$this->assertEqual($html,  $res);
	}
	
	function testBuildCell() {
		$html = '';
		$res  = $this->flash->buildCell($this->field, $this->object);
		$this->assertEqual($html,  $res);
	}
}
?>