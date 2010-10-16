<?php


class AclTest extends UnitTestCase {
	function setUp() {
		$e = new PXEngineSbin();
		$this->acl = new PXObjectsACL(PXRegistry::getDB(), PXRegistry::getUser());
	
		$sql  = "insert into acl_objects (objecttype, what, access, objectrule) values ('article', 'test_add',    'deny', 'user');\n";
		$sql .= "insert into acl_objects (objecttype, what, access, objectrule) values ('article', 'test_delete', 'allo', 'user');\n";

		PXRegistry::getDB()->modifyingQuery($sql);
	}

	function tearDown() {
		$sql  = "delete from acl_objects where what like '%test%';\n";
		PXRegistry::getDB()->modifyingQuery($sql);
	}
	
	function testCheck() {

		// Проверяем работоспособность при отключенном ACL
		$this->acl->reload();
		$this->acl->aclEnabled = false;

		$this->assertTrue($this->acl->check('delete', null, null));
		$this->assertTrue($this->acl->check('add', PXRegistry::getApp()->types['article'], array('id' => 20, 'parent' => 40)));

		// Проверяем работоспособность при включенном ACL
		$this->acl->aclEnabled  = true;
		$this->acl->reload();

		$r = $this->acl->check('delete', new PXTypeDescription, array('id'=>null, 'parent'=>null));
		$this->assertFalse($r);

		// $r = $this->acl->check('test_add', 'article', array('id'=>null, 'parent'=>null));
		// $this->assertFalse($r);

		$type = PXRegistry::getApp()->types['article'];
		$r = $this->acl->check('test_delete', $type, array('id'=>null, 'parent'=>null));
		$this->assertTrue($r);

	}
}

?>
