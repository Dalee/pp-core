<?php

class UserTest extends UnitTestCase {

	function setUp() {/*{{{*/
		$this->u = new PXUserCron();
		
		Mock::generate('PXDataBase');
		Mock::generate('PXApplication');
		Mock::generate('PXRequestUser');

		$this->d = new MockPXDataBase();
		$this->a = new MockPXApplication();
		$this->r = new MockPXRequestUser();

		$this->a->authrules = array("secure"=>array("enabled"=>true));
	}/*}}}*/ 
	
	function testCheckAuth() {/*{{{*/
		$this->r->setReturnValueAt(0, 'getVar', 'admin');
		$this->r->setReturnValueAt(1, 'getVar', '1010');

		$this->r->setReturnValueAt(0, 'getCookieVar', 'admin');
		$this->r->setReturnValueAt(1, 'getCookieVar', PXAuthSecure::encodePasswd('1010'));

		$this->d->setReturnValueAt(0, 'getObjectsByFieldLimited', array(
			"1"=>$ext = array(
				"id"=>1, 
				"title"=>"admin",
				"access"=>16384,
				"passwd"=>PXAuthSecure::passwdToDb("1010"))));

		$this->u->checkAuth();

		$this->assertEqual($this->u->id, 1);
		$this->assertEqual($this->u->login, "admin");
		$this->assertEqual($this->u->level, 16384);

		$this->assertIdentical($this->u->data, $ext);
	}
/*}}}*/

	function testAclType() {/*{{{*/
		$r = $this->u->aclType();
		$this->assertEqual($r, "basic");

		$this->u->groups = array(1,2,3);
		
		$r = $this->u->aclType();
		$this->assertEqual($r, "bygroup");
	}/*}}}*/

	//how it test?
	function testCan() {/*{{{*/
		//...
	}/*}}}*/

	function testIsAuthed() {/*{{{*/
		$this->u->id = 1;
		$this->assertTrue($this->u->isAuthed());
	}/*}}}*/

	function testGetAuthMethods() {/*{{{*/
		$r = $this->u->getAuthMethods();
		$this->assertIdentical(array("0"=>"secure"), $r);
	}/*}}}*/

	function testGetPrimaryAuthMethod() {/*{{{*/
		$r = $this->u->getPrimaryAuthMethod();
		$this->assertEqual("PXAuthSecure", $r);
	}/*}}}*/
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
