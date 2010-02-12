<?php

class AppTest extends UnitTestCase {
	function setUp() {
		$this->app = PXApplication::getInstance("PXEngineIndex");
	}
	
	function testGetInstance() {/*{{{*/
		$engines  = array("sbin", "json", "sbin", "action", "adminAction", "adminIndex", "adminPopup");

		while($e  = array_shift($engines)) 
			$this->assertIsA(PXApplication::getInstance("PXEngine".ucfirst($e)), "PXApplication");

	}/*}}}*/

	function testParseProperties() {
	}

	function testloadXMLDirectory() {
	}
	
	function testGetAllowedChilds() {
	}
	
	function testGetAllowedChildsKeys() {
	}

	function testGetAvailableModules() {/*{{{*/
		foreach($this->app->getAvailableModules(16384) as $k=>$v) {
			$this->assertIsA($v, "PXModuleDescription");
		}
	}/*}}}*/

	function testGetProperty() {/*{{{*/
		$app = PXApplication::getInstance("PXEngineSbin");
		
		$pone  = $app->getProperty("NON_EXISTING_TEST_PROPERTY",  1);
		$pzero = $app->getProperty("EXISTING_TEST_PROPERTY0", "**FAILURE**");
		
		$this->assertEqual($pone, 1);
		$this->assertEqual($pzero, 0);
	}/*}}}*/

	function testInitContentObject() {/*{{{*/
		$app = PXApplication::getInstance("PXEngineSbin");
		
		$r = $app->initContentObject("suser");
		
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
		
		$expected= array_combine($k = array("id", "parent", "title", "passwd", "realname", "email", "access", "data", "status"), 
			array_pad(array(), sizeof($k), null));

		$this->assertIdentical($r, $expected);
	}/*}}}*/
}

?>
