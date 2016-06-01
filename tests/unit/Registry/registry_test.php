<?php
require_once BASEPATH . '/libpp/lib/Debug/functions.inc';
require_once BASEPATH . '/libpp/lib/registry.class.inc';
require_once BASEPATH . '/libpp/lib/maincommon.inc';

use PP\Lib\Html\Layout\AdminHtmlLayout;

class RegistryTest extends UnitTestCase {
	function setUp() {
		$e = new PXEngineIndex();
	}

	function testGet() {
		$mapping = array(
			'app'     => 'PXApplication',
			'db'      => 'PXDataBase',
			'request' => 'PXRequest',
			'user'    => 'PXUserAuthorized'
		);

		foreach($mapping as $methodName => $objClass) {
			$method = 'get'.$methodName;
			$obj =& PXRegistry::$method();
			$this->assertIsA($obj, $objClass);
		}
	}

	function testSet() {
		PXRegistry::setLayout(new AdminHtmlLayout('index', PXRegistry::getTypes()));
		$this->assertIsA(PXRegistry::getLayout(), 'PP\Lib\Html\Layout\AdminHtmlLayout');

		PXRegistry::setUser(new PXUserNull());
		$this->assertIsA(PXRegistry::getUser(), 'PXUserNull');
	}
}
