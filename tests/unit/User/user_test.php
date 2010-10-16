<?php
class UserTest extends UnitTestCase {
	function setUp() {
		$e = new PXEngineIndex();
		$this->u = PXRegistry::getUser();
	
		$sql = "insert into suser (title, access, passwd, status) values ('unit_test_user', 16384, md5('testpasswd'), true)";
		PXRegistry::getDB()->modifyingQuery($sql);
	} 

	function tearDown() {
		$sql = "delete from suser where title like '%unit_test%'";
		PXRegistry::getDB()->modifyingQuery($sql);
	}
	
	function testCheckAuth() {
		$r = PXRegistry::getRequest();

		$r->setVar('login',  'unit_test_user');
		$r->setVar('passwd', 'testpasswd');

		$this->u->checkAuth();

		$this->assertTrue($this->u->isAuthed());
		$this->assertEqual($this->u->login,  "unit_test_user");
		$this->assertEqual($this->u->passwd, md5("testpasswd"));
		// $this->assertEqual($this->u->level, 16384);

		//need check auth by cookie
	}

	function testAclType() {
		$r = $this->u->aclType();
		$this->assertEqual($r, "basic");

		$this->u->groups = array(1,2,3);
		
		$r = $this->u->aclType();
		$this->assertEqual($r, "bygroup");
	}

	//how it test?
	function testCan() {
		//...
	}

	function testIsAuthed() {
		$this->u->id = 1;
		$this->assertTrue($this->u->isAuthed());
	}

	function testGetAuthMethods() {
		$r = $this->u->getAuthMethods();
		$this->assertIdentical(array("0"=>"secure"), $r);
	}

	function testGetPrimaryAuthMethod() {
		$r = $this->u->getPrimaryAuthMethod();
		$this->assertEqual("PXAuthSecure", $r);
	}
}

class CronUserTest extends UnitTestCase {

	function setUp() {
		$this->u = new PXUserCron();
	}

	function testAclType() {
		$this->assertEqual($this->u->aclType(), "GOD");
	}
}
?>