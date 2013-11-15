<?php

class UserRequestTest extends UnitTestCase {

	function setUp() {
		$_POST['area'] = 'test_place';
		$_POST['sid'] = 1;
		$_POST['cid'] = 1;
		$_POST['id'] = 1;
		$_POST['format'] = 'some';
		$_POST['parent'] = 1;
		$_POST['action'] = 'some';

		$_SERVER['HTTP_X_HOST'] = "test.ru";
		$_SERVER['SCRIPT_NAME'] = '/test/script/name.html';
		$_SERVER['REQUEST_METHOD'] = "POST";
		$_COOKIE['leafStatus'] = array("some"=>"close", "another"=>"open", "some another"=>"open");
	
		$_POST['test_datatype_title'] = "title_from_request";

		$_POST['allowed'] = array("suser", "muser");
		$_POST['sys_accessmod'] = 1;
		$_POST['sys_accessput'] = 2;

		$_POST['part'] = 'part';
		$_POST['test_datatype_order'] = "test_datatype_title DESC";

		$this->object = new PXRequest;
	}


	//сие множество тупых гет-тестов создано  для проверки того что данные методы в качестве 
	//переменной из реквеста получают именно те именна что должны быть	
	function test_get_area() {
		$this->assertEqual("test_place", $this->object->getArea());
	}

	function test_get_leaf_status() {
		//substr 6 why?
		$this->assertEqual(array("r", "nother"), $this->object->getLeafStatus());
	}

	function test_get_sid() {
		$this->assertEqual(1, $this->object->getSid());
	}
	
	function test_get_cid() {
		$this->assertEqual(1, $this->object->getCid());
	}
	
	function test_get_id() {
		$this->assertEqual(1, $this->object->getId());
	}
	
	function test_get_format() {
		$this->assertEqual('some', $this->object->getFormat());
	}
	
	function test_get_parent() {
		$this->assertEqual(1, $this->object->getParent());
	}
	
	function test_get_action() {
		$this->assertEqual('some', $this->object->getAction());
	}

	#studing reference format ...
	function test_get_links() {
	}

	function test_get_content_object() {
		$app = $this->init_test_datatype();
		$actual = $this->object->getContentObject($app->types['test_datatype']);

		$expected = "title_from_request";

		$this->assertTrue(isset($actual['test_datatype_title']));
		$this->assertEqual($expected, $actual['test_datatype_title']);
		
		$this->assertTrue(isset($actual['test_datatype_checkbox']));
		$this->assertFalse($actual['test_datatype_checkbox']);
	}

	function test_get_object_sys_vars() {
		$object = $this->object->getObjectSysVars(null, array(OBJ_FIELD_CHILDREN));

		$this->assertEqual(array("id"=>1, "allowed"=>array("suser", "muser"), "sys_accessmod" => 1, "sys_accessput" => 2), 
			$object);
	}

	function test_get_after_action_deal() {
		$this->assertEqual('back', $this->object->getAfterActionDeal());
		$this->object->postData->raw['close'] = 1;
		$this->assertEqual('close', $this->object->getAfterActionDeal());
	}

	function test_get_host_and_dir() {
	 	$actual = $this->object->getHostAndDir();
		$this->assertEqual(array("test.ru", "test", "script"), $actual);
	}

	function test_get_file() {
		$this->assertEqual('name.html', $this->object->getFile());

		$this->object->path = null;
		$this->assertEqual('index.html', $this->object->getFile());
	}

	function test_get_part() {
		$this->assertEqual('part', $this->object->getPart());
		unset($this->object->postData->raw['part']);
		$this->assertEqual('index.html', $this->object->getPart());
	}

	function test_get_order_var_id_exist_order_var_and_desc() {
		$app = $this->init_test_datatype();
		$actual = $this->object->getOrderVar('test_datatype', 'test_datatype_checkbox DESC', 
			$app->types['test_datatype']->fields);

		$this->assertEqual('test_datatype_title DESC', $actual);
	}

	function test_get_order_var_if_exist_order_var_and_asc() {
		$app = $this->init_test_datatype();
		
		$this->object->postData->raw['test_datatype_order'] = "test_datatype_title asc";
	
		$actual = $this->object->getOrderVar('test_datatype', 'test_datatype_checkbox ASC', 
			$app->types['test_datatype']->fields);

		$this->assertEqual('test_datatype_title', $actual);
	}

	function test_get_order_var_id_not_exist_order_var() {
		$app = $this->init_test_datatype();
		unset($this->object->postData->raw['test_datatype_order']);

		$actual = $this->object->getOrderVar('test_datatype', 'test_datatype_checkbox ASC', 
			$app->types['test_datatype']->fields);

		//HMMM!
		$this->assertEqual('test_datatype_checkbox ASC', $actual);
	}

	private function init_test_datatype() {/*{{{*/
		//Mock::generate("PXApplication");
		//$app = new MockPXApplication();
		$app = PXApplication::getInstance(new PXEngineIndex());

		$xml = <<<XML
<model>
	<datatypes>
		<datatype name="test_datatype" description="test">
			<attribute name="test_datatype_title" description="" storagetype="string" displaytype="TEXT" listed="true" />
			<attribute name="test_datatype_checkbox" description="" storagetype="boolean" displaytype="CHECKBOX" />
		</datatype>
	</datatypes>
</model>
XML;

		$dom = PXML::loadString($xml);
		PXTypeDescription::fillAppTypes($dom->xpath("/model/datatypes/datatype"), $app);
		return $app;
	}/*}}}*/

}

?>
