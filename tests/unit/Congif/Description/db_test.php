<?php

class DBDescriptionTest extends UnitTestCase {
	//check test runed
	/*function testTrue() {
		$this->assertTrue(false);
	}*/

	function setUp() {
		$this->dbd = new NLDBDescription();
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

		$map = array(
			"host"=>"host1",
			"port"=>"9999",
			"user"=>"web",
			"passwd"=>"123",
			"dbname"=>"test");

		foreach(get_object_vars($this->dbd) as $k=>$i) {
			if(isset($map[$k])) $this->dbd->$k = $map[$k];
		}
		$this->dbd->password = $map["passwd"];

		//sprintf black belt ;-)
		$ff = call_user_func_array("sprintf", 
			array(implode("", array_pad(array(), count($map), "\s*(#key#\s*=\s*%s)\s*?"))) + $map);
		
		$rr = call_user_func_array("sprintf", 
			array("format"=>strtr($ff, array("#key#"=>"%s"))) + array_keys($map));

		$r = $this->dbd->getPgsqlConnectString();
		$this->assertPattern("#^{$rr}$#", $r);
	}

	function testGetMssqlConnectArray() {
		$r = $this->dbd->getMssqlConnectArray();
		
		$expected = array(
			"host"=>"LOCALHOST",
			"port"=>"0",
			"user"=>"admin",
			"password"=>"",
			"dbname"=>"test");

		$this->assertIdentical($r, $expected);
	}

	function testGetMysqlConnectArray() {
		$r = $this->dbd->getMysqlConnectArray();
		
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
