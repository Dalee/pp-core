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
        /*
            '<div class="imagepreview"><input class="file" type="file" id="test_field" name="test_field" onpropertychange="Preview(this.value, \'file\', \'test_field\');">';
            '<br>';
            '<input type="text" readonly value="fname" ><span>имя</span>';
            '<input type="text" readonly value="fsize" ><span>размер</span>';
            '<input type="text" readonly value="ftype" ><span>тип</span>';
            '<br>';
            '<a href="fullpath" target="_blank" class="img-preview">просмотр</a> ';
            '<button onclick="return ToClipboardMulti(\'fullpath\', 0, 0, \'file\')">HTML в буфер</button>';
            '<br>';
            '<input class="checkbox" type="checkbox" id="test_field_remove" name="test_field_remove[ullpath]">';
            '<label for="test_field_remove">удалить файл</label>';
            '</div>';
         */

        $res  = $this->file->buildInput($this->field, $this->object);
        
        //enclosed with <div..>...</div>
        $this->assertPattern('#^<div[^>]+>.+</div>$#sm',  $res);
        //has input type=file
		$this->assertPattern('/input[^>]+type="file" id="test_field" name="test_field"/m',  $res);
        //has js hook
		$this->assertPattern('/onpropertychange="Preview\([^\)]+,\s+\'file\',\s+\'test_field\'/m',  $res);
        //has other inputs
        foreach(array('fname', 'fsize', 'ftype') as $field) {
            $this->assertPattern("/<input type=\"text\" readonly value=\"$field\"/im",  $res);
        }
        // has clipboard link
		$this->assertPattern('/<button onclick="ToClipboardMulti\([^\)]+\)/im',  $res);
        //jas remove checkbox
        $this->assertPattern('/<input.+ type="checkbox" id="test_field_remove" name="test_field_remove\[ullpath\]"/m', $res);
        //tags balanced
        $this->assertEqual(substr_count($res, '<'), substr_count($res, '>'));
	}
	
	function testBuildCell() {
		$html = '<a href="fullpath" class="file fname">fname</a> 0Кб</br>';
		$res  = $this->file->buildCell($this->field, $this->object);
		$this->assertEqual($html,  $res);
	}
}
?>
