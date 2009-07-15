<?php

/*{{{
	<datatype name="suser" description="Пользователь" orderby="title" parent="sgroup" bydefault="all">
		<attribute name="id"          description="ID"                  storagetype="pk"          displaytype="HIDDEN"/>
		<attribute name="parent"      description="Входит в группу"     storagetype="integer"     displaytype="PARENTDROPDOWN" source="sgroup" listed="true"/>
		<attribute name="title"       description="Логин"               storagetype="string"      displaytype="TEXT"/>
		<attribute name="passwd"      description="Пароль"              storagetype="string"      displaytype="PASSWORD" listed="true"/>
		<attribute name="realname"    description="Имя"                 storagetype="string"      displaytype="TEXT"/>
		<attribute name="email"       description="E-mail"              storagetype="string"      displaytype="TEXT"/>
		<attribute name="access"      description="Доступ"              storagetype="integer"     displaytype="DROPDOWN"  listed="true" source="suser-access"/>
		<attribute name="data"        description="MISC"                storagetype="serialized"  displaytype="STATIC"/>
		<attribute name="status"      description="Активен"             storagetype="boolean"     displaytype="CHECKBOX"/>
	</datatype>
}}}*/

class AppTest extends UnitTestCase {
	function setUp() {
		$e = new PXEngineSbin();
		$this->app = PXRegistry::getApp();
	}

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
		$ext = array("type", "var", "value", "module", "order", "pOrder");
		sort($ext);

		$this->assertIsA($bids = $this->app->modBindings, "array");

		foreach($bids as $i) {
			$md_k = array_keys($i);
			sort($md_k);
			$this->assertIdentical($md_k, $ext);
		}
	}

	function testGetProperty() {
		$r[] = $this->app->getProperty("NOT_SUCH", 1);
		$r[] = $this->app->getProperty("PROFILER", 1);

		$this->assertEqual(array_sum($r) / sizeof($r), 1);
	}

	function testInitContentObject() {
		$r = $this->app->initContentObject("suser");
		
		$expected= array_combine($k = array("id", "parent", "title", "passwd", "realname", "email", "access", "data", "status"), 
			array_pad(array(), sizeof($k), null));

		$this->assertIdentical($r, $expected);
	}
	
	function testParseProperties() {
		$r = $this->app->parseProperties();
		$expt = array("PROFILER"=>"1", "CHILDREN_ON_PAGE"=>"10");
		$this->assertIdentical($r, $expt);
	}

	function testGetAvailableModules() {
		//admin
		$r = $this->app->getAvailableModules(16384);
		$this->checkStruct($r, new PXModuleDescription());
	}
}	

?>
