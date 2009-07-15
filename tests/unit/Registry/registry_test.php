<?php
require_once BASEPATH . '/libpp/lib/Debug/functions.inc';
require_once BASEPATH . '/libpp/lib/registry.class.inc';
require_once BASEPATH . '/libpp/lib/maincommon.inc';
require_once BASEPATH . '/libpp/lib/HTML/Abstract/layout.class.inc';
require_once BASEPATH . '/libpp/lib/HTML/Admin/layout.class.inc';

class RegistryTest extends UnitTestCase {
	function setUp() {
		$e =& new PXEngineSbin(); $e->init();
	}
	
	function testInstance() {
		$registry =& PXRegistry::instance();
		$this->assertIsA($registry, 'PXRegistry');
	}

	function testGet() {
		$mapping = array(
			'app'     => 'PXApplication', 
			'db'      => 'PXDataBase', 
			'request' => 'PXNullRequest', //checked parent 
			'user'    => 'PXUserCron'
		);
	
		foreach($mapping as $methodName => $objClass) {
			$method = 'get'.$methodName;
			$obj =& PXRegistry::$method();
			$this->assertIsA($obj, $objClass);
		}
	}

	function testSet() {
		PXRegistry::setLayout(new PXAdminHTMLLayout('index', PXRegistry::getTypes()));
		$this->assertIsA(PXRegistry::getLayout(), 'PXAdminHTMLLayout');

		PXRegistry::setUser(new PXUserNull());
		$this->assertIsA(PXRegistry::getUser(), 'PXUserNull');
	}

	//deep
	/*function test_ObjectsIdentity() {
		foreach(array('app', 'db', 'request', 'response', 'user', 'layout') as $key) {
			$method = 'get'.$key;
			$obj1 =& PXRegistry::$method();
			$obj2 =& PXRegistry::$method();

			$this->assertIdentical($obj1, $obj2);
		}
	}*/
}
?>
