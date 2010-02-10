<?php
class DBDescriptionTest extends UnitTestCase {
	var $map1 = array(
			"host"   => "host1",
			"port"   => "9999",
			"user"   => "web",
			"passwd" => "123",
			"dbname" => "test");

	function setUp() {
		$this->dbd = new NLDBDescription($this->map1);
	}

	function testGetDriver() {
		$map = array(
			"pgsql" => "nlpgsqldatabase",
			"mssql" => "nlmssqldatabase",
			"mysql" => "nlmysqldatabase");

		foreach($map as $type=>$expected) {
			$this->dbd->dbtype  = $type;
			$driver =& $this->dbd->getDriver();
			$this->assertIsA($driver, $expected);
		}
	}

	function testGetPgsqlConnectString() {
		$r = $this->dbd->getPgsqlConnectString();
		$this->assertTrue(empty($r));

		foreach(get_object_vars($this->dbd) as $k=>$i) {
			if(isset($this->map1[$k])) $this->dbd->$k = $this->map1[$k];
		}
		$this->dbd->password = $this->map1['passwd']; //dirty hack 

		//sprintf black belt ;-)
		$ff = call_user_func_array("sprintf", 
			array(implode("", array_pad(array(), count($this->map1), "\s*(#key#\s*=\s*%s)\s*?"))) + $this->map1);
		
		$rr = call_user_func_array("sprintf", 
			array("format"=>strtr($ff, array("#key#"=>"%s"))) + array_keys($this->map1));

		$r = $this->dbd->getPgsqlConnectString();
		$this->assertPattern("#^{$rr}$#", $r);
	}

	function testGetMssqlConnectArray() {
		$dbd = new NLDBDescription(array());
		$r = $dbd->getMssqlConnectArray();
		
		$expected = array(
			"host"=>"LOCALHOST",
			"port"=>"0",
			"user"=>"admin",
			"password"=>"",
			"dbname"=>"test");

		$this->assertIdentical($r, $expected);
	}

	function testGetMysqlConnectArray() {
		$dbd = new NLDBDescription(array());
		$r = $dbd->getMysqlConnectArray();
		
		$expected = array(
			"host"=>"localhost",
			"port"=>"3306",
			"user"=>"admin",
			"passwd"=>"",
			"dbname"=>"test");

		$this->assertIdentical($r, $expected);
	}
}
?>