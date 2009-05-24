<?php
class PasswordTest extends UnitTestCase {
	
	function setUp() {
		Mock::generate('PXFieldDescription');
		$this->field       = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		
		$this->object = array('test_field' => md5('qwerty'));
		
		$this->password = new PXDisplayTypePassword($this->field, 'password');
		
		$this->UnitTestCase();
	}
	
	function testBuildInput() {
		$html  = '<div class="password">';
		$html .= '<input type="password" name="test_field[type]"   onfocus="startCheckPassword(this, \'test_field\')">';
		$html .= '<input type="password" name="test_field[retype]" onfocus="startCheckPassword(this, \'test_field\')">';
		$html .= '<input type="checkbox" name="test_field[delete]" id="test_field[delete]" class="checkbox"><label for="test_field[delete]">������� ������</label>';
		$html .= ' <span>����� ������ ������ ���� �� ����� 6 ��������. ������� ������ ������ �������� � ���� �������� � ��������� ����� ���������� ��������, � ����� �����.</span>';
		$html .= '</div>';
		
		$res  = $this->password->buildInput($this->field, $this->object);
		$this->assertEqual($html,  $res);
		
		// ����� ������ < 32 
		$this->object['test_field'] = 'qwerty';
		$html  = str_replace('<input type="checkbox" name="test_field[delete]" id="test_field[delete]" class="checkbox"><label for="test_field[delete]">������� ������</label>', '', $html);
		
		$res  = $this->password->buildInput($this->field, $this->object);
		$this->assertEqual($html,  $res);
	}
	
	function testBuildCell() {
		$html = '<div class="truecheck" title="��"></div>';
		$this->object['test_field'] = md5('qwerty');
		
		$res  = $this->password->buildCell($this->field, $this->object);
		$this->assertEqual($res,  $html);
		
		
		$html = '';
		$this->object['test_field'] = 'qwerty';
		
		$res  = $this->password->buildCell($this->field, $this->object);
		$this->assertEqual($res,  $html);
	}
	
	function testGetFromRequest() {
		$request = new NLRequest();
		$var     = new NLGetData();
		$var->raw['test_field'] = array ('type' => 'qwerty', 'retype' => 'qwerty');
		
		$request->postData = & $var;
		$param['request'] = $request;
		
		$res = $this->password->getFromRequest($this->field, $this->object, $param);
		$this->assertEqual($res, md5('qwerty'));
		
		$var->raw['test_field']['delete'] = 'on';
		
		$res = $this->password->getFromRequest($this->field, $this->object, $param);
		$this->assertEqual($res, "\n\t\n");
	}
}
?>