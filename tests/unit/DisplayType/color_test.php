<?php
class ColorTest extends UnitTestCase {
	
	function setUp() {
		Mock::generate('PXFieldDescription');
		$this->field       = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		
		$this->object = array('test_field' => '000001');
		
		$this->color = new PXDisplayTypeColor($this->field, 'color');
		
		$this->UnitTestCase();
	}
	
	function testBuildCell() {
		$color = '<div class="rgbcolor"><div style="background-color: #000001;" title="#000001"></div></div>';
		
		$res = $this->color->buildCell($this->field, $this->object);
		$this->assertEqual($res, $color);
	}
	
	function testBuildInput() {
		$html  = '<div class="colorselect"><div id="colorBGtest_field" style="background-color: #000001;"></div>';
		$html .= '<input type="text" name="test_field" id="colorDatatest_field" value="000001"  readonly>';

		$html .= '<input type="hidden" id="colorRedtest_field"    value="0">';
		$html .= '<input type="hidden" id="colorGreentest_field"  value="0">';
		$html .= '<input type="hidden" id="colorBluetest_field"   value="1">';

		$html .= '<button title="изменить цвет"';
		$html .= ' onclick="with(this.form)ctrlColorer.DoModal(this.form.colorDatatest_field, this.form.colorRedtest_field, this.form.colorGreentest_field, this.form.colorBluetest_field, document.getElementById(\'colorBGtest_field\'));"';
		$html .= ' onFocus="this.blur();"></button>';

		$html .= '<iframe src="tools/colorer/index.html" name="colortest_field" id="colortest_field" scrolling="no" frameborder="0"></iframe>';
		$html .= '</div>';
		
		$res = $this->color->buildInput($this->field, $this->object);
			
		$this->assertEqual($res, $html);
	}
}
?>