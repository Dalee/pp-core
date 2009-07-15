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
		$res  = $this->image->buildInput($this->field, $this->object);
        
        //enclosed with <div..>...</div>
        $this->assertPattern('#^<div[^>]+>.+</div>$#sm',  $res);
        //has input type=file
		$this->assertPattern('/input[^>]+type="file" id="test_field" name="test_field"/m',  $res);
        //has js hook
		$this->assertPattern('/onpropertychange="Preview\([^\)]+, \'image\', \'test_field\'/m',  $res);
        //has other inputs
		$this->assertPattern('/input id="test_field(width|height|size)".+readonly/m',  $res);
        //tags balanced
        $this->assertEqual(substr_count($res, '<'), substr_count($res, '>'));
	}
	/*  TODO: check me
        function testBuildCell() {
            $html = '<div class="imagepreview image-in-cell"><div class="small"></div></div>';
            $res  = $this->image->buildCell($this->field, $this->object);
        }
    */
}
?>
