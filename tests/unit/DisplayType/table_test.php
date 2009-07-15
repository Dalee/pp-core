<?php
class TableTest extends UnitTestCase {
	
	function setUp() {
		Mock::generate('PXFieldDescription');
		$this->field       = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		$this->field->displayTypeArgs[0] = '1-2-1-1';
		
		$this->object = array('test_field' => array());
		
		$this->table = new PXDisplayTypeTable($this->field, 'table');
		
		$this->UnitTestCase();
	}
	
    /*
     *  FIXME !!!!
	
	function testBuildInput() {
		$this->field->displayTypeArgs[1] = 0;
		
		$text  = '<table id="tabletest_field" class="datagrid"><colgroup><col width="20%"><col width="40%"><col width="20%"><col width="20%"></colgroup>';
		$text .= '<tr><td><input type="text" name="test_field[0][]" value="" ></td>';
		$text .= '<td><input type="text" name="test_field[0][]" value="" ></td>';
		$text .= '<td><input type="text" name="test_field[0][]" value="" ></td>';
		$text .= '<td><input type="text" name="test_field[0][]" value="" ></td>';
		$text .= '</tr></table><button class="addrow" onclick="AddRowNew(\'test_field\'); return false;">Добавить</button>';
		
		$res  = $this->table->buildInput($this->field, $this->object);
		$this->assertEqual($text,  $res);
		
		$this->field->displayTypeArgs[1] = 1;
		
		$textarea = '<table id="tabletest_field" class="datagrid"><colgroup><col width="20%"><col width="40%"><col width="20%"><col width="20%"></colgroup>';
		$textarea .= '<tr><td><textarea name="test_field[0][]" wrap="physical" style="height:1px;"></textarea></td>';
		$textarea .= '<td><textarea name="test_field[0][]" wrap="physical" style="height:1px;"></textarea></td>';
		$textarea .= '<td><textarea name="test_field[0][]" wrap="physical" style="height:1px;"></textarea></td>';
		$textarea .= '<td><textarea name="test_field[0][]" wrap="physical" style="height:1px;"></textarea></td>';
		$textarea .= '</tr></table><button class="addrow" onclick="AddRowNew(\'test_field\'); return false;">Добавить</button>';
		
		$res  = $this->table->buildInput($this->field, $this->object);
		$this->assertEqual($textarea,  $res);
    }
     */
}
?>
