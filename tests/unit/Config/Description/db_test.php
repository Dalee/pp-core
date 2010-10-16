<?php

class DBDescriptionTest extends UnitTestCase {

	var $map1 = array(
		"host"   => "host1",
		"port"   => "9999",
		"user"   => "web",
		"password" => "123",
		"dbname" => "test"
	);

	function setUp() {
		$this->dbd = new NLDBDescription($this->map1);
	}

	function testGetDriver() {
		$map = array(
			"pgsql" => "nlpgsqldatabase",
			"mssql" => "nlmssqldatabase",
			"mysql" => "nlmysqldatabase"
		);

		foreach($map as $type=>$expected) {
			$this->dbd->dbtype  = $type;
			$driver =& $this->dbd->getDriver();
			$this->assertIsA($driver, $expected);
		}
	}

	function testGetPgsqlConnectString() {
		$r = $this->dbd->getPgsqlConnectString();
		$this->assertEqual("host={$this->map1['host']} port={$this->map1['port']} user={$this->map1['user']} password={$this->map1['password']} dbname={$this->map1['dbname']}", $r);
	}

	function testGetMssqlConnectArray() {
		$r = $this->dbd->getMssqlConnectArray();
		
		$expected = array(
			"host"     => $this->map1['host'],
			"port"     => $this->map1['port'],
			"user"     => $this->map1['user'],
			"password" => $this->map1['password'],
			"dbname"   => $this->map1['dbname'],
		);

		$this->assertIdentical($r, $expected);
	}

	function testGetMysqlConnectArray() {
		$r = $this->dbd->getMysqlConnectArray();

		$expected = array(
			"host"     => $this->map1['host'],
			"port"     => $this->map1['port'],
			"user"     => $this->map1['user'],
			"passwd" => $this->map1['password'],
			"dbname"   => $this->map1['dbname'],
		);

		$this->assertIdentical($r, $expected);
	}

}

?>