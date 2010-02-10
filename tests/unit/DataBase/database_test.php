<?php
class DatabaseTest extends UnitTestCase {

	function setUp() {/*{{{*/ 
		$e = new PXEngineSbin();

		$this->app = PXRegistry::getApp();
		$this->db  = PXRegistry::getDb();

		//testins object /suser/
		$this->dt = $this->app->types['suser'];
	}/*}}}*/

	function testDefines() {/*{{{*/
		foreach(array(DB_SELECT_TABLE, DB_SELECT_COUNT, DB_SELECT_TREE, DB_SELECT_LIST, DB_SELECT_FULL_TREE) as $k => $i) {
			$this->assertEqual($k, $i);
		}
	}/*}}}*/

	/**
	 * Проверяем объекты необходимые для работы 
	 **/ 
	function testIsObject() {/*{{{*/
		$this->assertIsA($this->db, "pxdatabase");
		$this->assertIsA($this->db, "nlsqldatabaseadapter");

		$this->assertIsA($this->db->app, "pxapplication");
		$this->assertIsA($this->db->user, "PXUserCron");

		$this->assertIsA($this->db->types, "array");
	}/*}}}*/ 

	/**
	 * проверяем кол-во переданных аргументов и их значение для коструктора
	 */
	function testPXDatabase() {/*{{{*/
	}/*}}}*/

	function testSetUser() {/*{{{*/
	}/*}}}*/

	function testSetCache() {/*{{{*/
	}/*}}}*/

	function testCacheOn() {/*{{{*/
	}/*}}}*/

	function testCacheOff() {/*{{{*/
	}/*}}}*/

	function testLoadTriggers() {/*{{{*/
		$this->db->loadTriggers();
		
		$this->assertIsA($this->db->triggers, "array");
		$this->assertIsA(current($this->db->triggers), "pxabstracttrigger");
	}/*}}}*/

	function testQuery() {/*{{{*/
		$result = $this->db->query("select parent from suser where parent is not null limit 1");
		$this->assertIsA($result, "array");
		$this->parent = current(current($result));
	 }/*}}}*/

	function case1($result, $check_size=false) {
		$this->assertIsA($result, "array");
	
		if($check_size) {
			$this->assertEqual(sizeof($result), 1, "Must be one");

			$suser_keys = array_keys($this->o);
			sort($suser_keys);

			foreach($result as $i) {
				$ks = array_keys($i);
				sort($ks);
				$this->assertIdentical($ks, $suser_keys);
			}
		}
	}
	
	function testAddContentObject() {
		$suser = $this->app->initObject("suser");

		$suser['title']  = "UnitTest";
		$suser['parent'] = $this->parent;
		$suser['status'] = true;
		$this->id = $result = $this->db->addContentObject($this->app->types['suser'], $suser);

		$this->assertPattern("#[0-9]+#", $result);
	}
	
	function testGetObjects() {
		$result = $this->db->getObjects($this->dt, true);
		$this->case1($result);
	}
	
	function testGetObjectById() {
		$result = $this->db->getObjectById($this->dt, $this->id);

		$this->o = $result;

		$this->assertIsA($result, "array");
		$this->assertFalse(is_a(current($result), "array"));
	}

	function testGetObjectByIdArray() {
		$result = $this->db->getObjectsByIdArray($this->dt, true, array($this->o['id']));
		$this->case1($result, true);
	}

	function testGetObjectsLimited() {
		$result = $this->db->getObjectsLimited($this->dt, true, 1, 0);
		$this->case1($result, true);
	}

	function testGetObjectByParent() {
		$result = $this->db->getObjectsByParent($this->dt, true, $this->o['parent']);
		$this->case1($result);
	}

	function testGetObjectsByParentLimited() {
		$result = $this->db->getObjectsByParentLimited($this->dt, true, $this->o['parent'], 1, 0);
		$this->case1($result, true);
	}

	function testGetObjectsByWhere() {
		$result = $this->db->getObjectsByWhere($this->dt, true, "parent  = {$this->o['parent']}");
		$this->case1($result);
	}

	function testGetObjectsByWhereLimited() {
		$result = $this->db->getObjectsByWhereLimited($this->dt, true, "parent  = {$this->o['parent']}", 1, 0);
		$this->case1($result, true);
	}

	function testGetObjectsByField() {
		$result = $this->db->getObjectsByField($this->dt, true, "parent", $this->o['parent']);
		$this->case1($result);
	}

	function testGetObjectsByFieldLimited() {
		$result = $this->db->getObjectsByFieldLimited($this->dt, true, "parent", $this->o['parent'], 1, 0);
		$this->case1($result, true);
	}

	function testGetObjectByFieldLike() {
		$result = $this->db->getObjectsByFieldLike($this->dt, true, "title", $this->o['title']);
		$this->case1($result);
	}

	function testGetObjectsByFieldLikeLimited() {
		$result = $this->db->getObjectsByFieldLikeLimited($this->dt, true, "title", $this->o['title'], 1, 0);
		$this->case1($result, true);
	}

	function testGetObjectsByFieldNotEmpty() {
		$result = $this->db->getObjectsByFieldNotEmpty($this->dt, true, "title");
		$this->case1($result);
	}
	
	function testGetObjectsByFieldNotEmptyLimited() {
		$result = $this->db->getObjectsByFieldNotEmptyLimited($this->dt, true, "title", 1, 0);
		$this->case1($result, true);
	}

	function testGetObjectsBySearchWord() {
		$result = $this->db->getObjectsBySearchWord($this->dt, true, $this->o['title']);
		$this->case1($result);
	}
	
	function testGetObjectsBySearchWordLimited() {
		$result = $this->db->getObjectsBySearchWordLimited($this->dt, true, $this->o['title'], 1, 0);
		$this->case1($result, true);
	}

	function testModifyContentObject() {
		$this->o['title'] = "AnotherUnitTest";
		$this->db->modifyContentObject($this->dt, $this->o);
		$r = $this->db->getObjectById($this->dt, $this->o['id']);
		$this->assertEqual($this->o['title'], $r['title']);
	}

	function testModifyObjectSysVars() {
		//...
	}

	function testUpContentObject() {
		$this->db->upContentObject($this->dt, $this->id);
		$result = $this->db->getObjectById($this->dt, $this->id);

		$this->assertTrue($this->o['sys_order'] > $result['sys_order']);

		$this->o = $result;
	}
	
	function testDownContentObject() {
		$this->db->downContentObject($this->dt, $this->id);
		$result = $this->db->getObjectById($this->dt, $this->id);

		$this->assertTrue($this->o['sys_order'] < $result['sys_order']);

		$this->o = $result;
	}
	
	function testDeleteContentObject() {
		$this->db->deleteContentObject($this->dt, $this->o['id']);
		$result = $this->db->getObjectById($this->dt, $this->o['id']);
		$this->assertTrue(empty($result));
	}
}

?>
