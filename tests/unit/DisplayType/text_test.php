<?php
class TextTest extends UnitTestCase {
	
	function setUp() {
		Mock::generate('PXFieldDescription');
		$this->field       = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		
		$this->text = new PXDisplayTypeText($this->field, 'text');
		
		$this->UnitTestCase();
	}
	
	function testBuildCell() {
		$resShort  = 'text';
		$resLong = 'text&hellip;';
		
		$object = array('test_field' => 'text');
		
		$tt = $this->text->buildCell($this->field, $object);
		$this->assertEqual($tt, $resShort);
		
		$object['test_field'] = 'text1';
		$this->field->displayTypeArgs[2] = 4;
		
		$tt = $this->text->buildCell($this->field, $object);
		$this->assertEqual($tt, $resLong);
	}
	
	function testBuildInput() {
        /*
            '<input type="text" name="test_field" value="text" >';
            '<textarea name="test_field" wrap="physical" style="height:10px;">text</textarea>';
         */
		$object = array('test_field' => 'text');
        
        $tt = $this->text->buildInput($this->field, $object);
		$this->assertPattern('#<input\s+type="text"\s+name="test_field"\s+value="text"#m', $tt);
		
		$this->field->displayTypeArgs[1] = 10;
		$tt = $this->text->buildInput($this->field, $object);
        $this->assertPattern('#<textarea\s+name="test_field".+style="height:\s*10px;"[^>]*>text</textarea>#m', $tt);
    }
}

?>
