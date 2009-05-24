<?php
class DelimiterTest extends UnitTestCase {
	
	function setUp() {
		Mock::generate('PXFieldDescription');
		$this->field       = new MockPXFieldDescription();
		$this->field->description = 'desctiption_text';
		
		$this->object = array();
		
		$this->color = new PXDisplayTypeDelimiter($this->field, 'delimiter');
		
		$this->UnitTestCase();
	}
	
	function testBuildCell() {
		$firstLine = "\n";
		$firstLine .= '<tr><td colspan="2" class="delim"><div><div class="lhr"><hr size="1" noshade="1"/></div><div>&nbsp;desctiption_text&nbsp;</div><div class="rhr"><hr size="1" noshade="1"/></div></div></td></tr>';
		
		$secontLine = str_replace('<tr>', '<tr class="even">', $firstLine);
		
		$resFirst  = $this->color->buildRow($this->field, $this->object);
		$resSecond = $this->color->buildRow($this->field, $this->object);
		$resThird  = $this->color->buildRow($this->field, $this->object);
		
		$this->assertEqual($resFirst,  $firstLine);
		$this->assertEqual($resSecond, $secontLine);
		$this->assertEqual($resThird,  $firstLine);
	}
}
?>