<?php


class AclTest extends UnitTestCase {
	function setUp() {
		$e = new PXEngineSbin();
		$this->acl = new PXObjectsACL(PXRegistry::getDB(), PXRegistry::getUser());
	
		$sql  = "insert into acl_objects (objecttype, what, access) values ('article', 'test_add', 'deny');\n";
		$sql .= "insert into acl_objects (objecttype, what, access) values ('article', 'test_delete', 'allo');\n";

		PXRegistry::getDB()->modifyingQuery($sql);
	}

	function tearDown() {
		$sql  = "delete from acl_objects where what like '%test%';\n";
		PXRegistry::getDB()->modifyingQuery($sql);
	}
	
	function testCheck() {
		$this->acl->reload();
		$this->acl->aclEnabled  = false;
		$r = $this->acl->check('delete', null, null);
		
		$this->assertTrue($r);
		
		$this->acl->aclEnabled  = true;
		$this->acl->reload();

		$r = $this->acl->check('delete', new PXTypeDescription , array('id'=>null, 'parent'=>null));
		$this->assertFalse($r);

		$type = PXRegistry::getApp()->types['article'];
		$r = $this->acl->check('test_add', $type, array('id'=>null, 'parent'=>null));
		$this->assertFalse($r);
		
		$r = $this->acl->check('test_delete', $type, array('id'=>null, 'parent'=>null));
		$this->assertTrue($r);
	}
}

?>
