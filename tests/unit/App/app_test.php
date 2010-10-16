<?php
class AppTest extends UnitTestCase {
	function setUp() {
		$this->app = PXApplication::getInstance(new PXEngineIndex());
	}

	function testGetInstance() {/*{{{*/
		// $engines  = array("sbin", "json", "sbin", "action", "adminAction", "adminIndex", "adminPopup");

		// while($e  = array_shift($engines)) 
		//	$this->assertIsA(PXApplication::getInstance("PXEngine".ucfirst($e)), "PXApplication");
	}/*}}}*/

	function testParseProperties() {
		$r = $this->app->properties;
		$expt = array(
			"PROFILER"                => "1",
			"CHILDREN_ON_PAGE"        => "10",
			"EXISTING_TEST_PROPERTY0" => "0",
			"EXISTING_TEST_PROPERTY1" => "1"
		);
		$this->assertIdentical($r, $expt);
	}

	function testloadXMLDirectory() {
	}

	function testGetAllowedChilds() {
	}

	function testGetAllowedChildsKeys() {
	}

	function testGetAvailableModules() {
	}

	function testGetProperty() {/*{{{*/
		$pone  = $this->app->getProperty("NON_EXISTING_TEST_PROPERTY",  1);
		$pzero = $this->app->getProperty("EXISTING_TEST_PROPERTY0", "**FAILURE**");

		$this->assertEqual($pone, 1);
		$this->assertEqual($pzero, 0);
	}/*}}}*/

	function testInitContentObject() {/*{{{*/
		$r = $this->app->initContentObject("suser");

		/* {{{ <datatype name="suser" description="Пользователь" orderby="title" parent="sgroup" bydefault="all">
			<attribute name="id"          description="ID"                  storagetype="pk"          displaytype="HIDDEN"/>
			<attribute name="parent"      description="Входит в группу"     storagetype="integer"     displaytype="PARENTDROPDOWN" source="sgroup" listed="true"/>
			<attribute name="title"       description="Логин"               storagetype="string"      displaytype="TEXT"/>
			<attribute name="passwd"      description="Пароль"              storagetype="string"      displaytype="PASSWORD" listed="true"/>
			<attribute name="realname"    description="Имя"                 storagetype="string"      displaytype="TEXT"/>
			<attribute name="email"       description="E-mail"              storagetype="string"      displaytype="TEXT"/>
			<attribute name="access"      description="Доступ"              storagetype="integer"     displaytype="DROPDOWN"  listed="true" source="suser-access"/>
			<attribute name="data"        description="MISC"                storagetype="serialized"  displaytype="STATIC"/>
			<attribute name="status"      description="Активен"             storagetype="boolean"     displaytype="CHECKBOX"/>
			</datatype> }}} */

		$expected = array_combine($k = array("id", "parent", "title", "passwd", "realname", "access", "status", "image", "file"), 
			array_pad(array(), sizeof($k), null));
		
		$this->assertIdentical($r, $expected);
	}/*}}}*/

	/**
	 * Проверить структуру массива 
	 * проверить структуру элемента
	 **/
	function checkStruct($in, $ext) {
		$this->assertIsA($in, "array");

		$map = array_keys(get_object_vars($ext));
		sort($map);

		foreach($in as $i) {
			$this->assertIsA($i, get_class($ext));

			$ex1 = array_keys(get_object_vars($i));
			sort($ex1);

			$this->assertIdentical($ex1, $map);
		}
	}

	function testTypes() {
		$this->checkStruct($this->app->types, new PXTypeDescription());
	}

	function testDescrpition() {
		$this->checkStruct($this->app->dbDescription, new NLDbDescription());
	}

	function testModules() {
		$this->checkStruct($this->app->modules, new PXModuleDescription());
	}

	function testDirectory() {
		$this->checkStruct($this->app->directory, new PXDirectoryDescription("suser"));
	}

	function testModBindings() {
	}
}
?>