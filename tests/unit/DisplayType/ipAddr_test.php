<?php
class IpAddrTest extends UnitTestCase {
	
	function setUp() {
		Mock::generate('PXFieldDescription');
		$this->field       = new MockPXFieldDescription();
		$this->field->name = 'test_field';
		
		$this->object = array('test_field' => '167903778');
		
		$this->ipAddr = new PXDisplayTypeIpaddr($this->field, 'ipAddr');
		
		$this->UnitTestCase();
	}
	
	function testBuildInput() {
        /*
            '<div class="ip">';
            '<input type="text" maxlength="3" name="test_field[0]" value="10" class="first">';
            '<input type="text" class="delim" value="." readonly tabindex="-1">';
            '<input type="text" maxlength="3" name="test_field[1]" value="2" class="middle">';
            '<input type="text" class="delim" value="." readonly tabindex="-1">';
            '<input type="text" maxlength="3" name="test_field[2]" value="2" class="middle">';
            '<input type="text" class="delim" value="." readonly tabindex="-1">';
            '<input type="text" maxlength="3" name="test_field[3]" value="34" class="last">';
            '</div>';
         */
		
		$res  = $this->ipAddr->buildInput($this->field, $this->object);
		$this->assertPattern('/input type="text".+name="test_field\[0\]"\s+value="10"/m',  $res);
		$this->assertPattern('/input type="text".+name="test_field\[1\]"\s+value="2"/m',  $res);
		$this->assertPattern('/input type="text".+name="test_field\[2\]"\s+value="2"/m',  $res);
		$this->assertPattern('/input type="text".+name="test_field\[3\]"\s+value="34"/m',  $res);
	}
	
	function testBuildCell() { //
		$cell = '10.2.2.34';
		
		$res  = $this->ipAddr->buildCell($this->field, $this->object);
		$this->assertEqual($cell,  $res);
	}
	
	function testGetFromRequest() {
		$request = new PXRequestBase();
		$var     = new NLGetData();
		
		$var->raw['test_field'] = array(10, 2, 2, 34);
		
		$request->postData = & $var;
		$request->getData = & $var;
		$param['request'] = $request;
		
		$res = $this->ipAddr->getFromRequest($this->field, $this->object, $param);
		$this->assertEqual($res, '167903778');
		
		$var->raw['test_field'] = '10.2.2.34';
		
		$res = $this->ipAddr->getFromRequest($this->field, $this->object, $param);
		$this->assertEqual($res, '10.2.2.34');
	}
}
?>
