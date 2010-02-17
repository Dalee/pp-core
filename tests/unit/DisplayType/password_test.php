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
		$html .= '<input type="checkbox" name="test_field[delete]" id="test_field[delete]" class="checkbox"><label for="test_field[delete]">удалить пароль</label>';
		$html .= ' <span>Длина пароля должна быть не менее 6 символов. Хороший пароль должен включать в себя строчные и заглавные буквы латинского алфавита, а также цифры.</span>';
		$html .= '</div>';
		
		$res  = $this->password->buildInput($this->field, $this->object);
        
        $this->assertPattern('#^\s*<div[^>]+>.+</div>\s*$#sm',  $res);
        $this->assertPattern('#input\s+type="password"\s+name="test_field\[type\]"#m',  $res);
        $this->assertPattern('#input\s+type="password"\s+name="test_field\[retype\]"#m',  $res);
        $this->assertPattern('#input\s+type="checkbox"\s+name="test_field\[delete\]"#m',  $res);
        
        
		// длина пароля < 32 
		$this->object['test_field'] = 'qwerty';
		$res  = $this->password->buildInput($this->field, $this->object);
        
        // search for <span>blabla</span> notice
		$this->assertPattern("#<span>[^<]+</span>#sm",  $res);
	}
	
	function testBuildCell() {
		$html = '<div class="truecheck" title="да"></div>';
		$this->object['test_field'] = md5('qwerty');
		
		$res  = $this->password->buildCell($this->field, $this->object);
		$this->assertEqual($res,  $html);
		
		
		$html = '';
		$this->object['test_field'] = 'qwerty';
		
		$res  = $this->password->buildCell($this->field, $this->object);
		$this->assertEqual($res,  $html);
	}
	
	function testGetFromRequest() {
		$request = new PXRequest();
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
