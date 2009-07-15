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
        /*
            '<div class="imagepreview">';
            '<div><script type="text/javascript">ShowFlash(\'i/l.swf?path=i/v.swf\', 100, 100, \'test_fieldsrc\');</script></div>';
            '<input class="file" type="file" id="test_field" name="test_field" onpropertychange="Preview(this.value, \'flash\', \'test_field\');">';
            '<br>';
            '<input id="test_fieldwidth"  type="text" readonly value="" ><span>ширина</span>';
            '<input id="test_fieldheight" type="text" readonly value=""><span>высота</span>';
            '<input id="test_fieldsize"   type="text" readonly value=""><span>размер</span>';
            '</div>';
         */

		$res  = $this->flash->buildInput($this->field, $this->object);
        
        //enclosed with <div..>...</div>
        $this->assertPattern('#^<div[^>]+>.+</div>$#sm',  $res);
        //has input type=file
		$this->assertPattern('/input[^>]+type="file" id="test_field" name="test_field"/m',  $res);
        //has js hook
		$this->assertPattern('/onpropertychange="Preview\([^\)]+,\s+\'flash\',\s+\'test_field\'/m',  $res);
        //has other inputs
		$this->assertPattern('/input id="test_field(width|height|size)".+readonly/m',  $res);
        //tags balanced
        $this->assertEqual(substr_count($res, '<'), substr_count($res, '>'));
	}
	
	function testBuildCell() {
		$html = '';
		$res  = $this->flash->buildCell($this->field, $this->object);
		$this->assertEqual($html,  $res);
	}
}
?>
