<?php
require_once BASEPATH . '/libpp/lib/Cache/object.class.inc';
require_once BASEPATH . '/libpp/lib/Common/functions.file.inc';

class ObjectCacheTest extends UnitTestCase {
	var $cache_domain = 'test_domain';
	var $object_id    = 'test-object-id';
	var $object_value = 'test-object-value';

	function setUp() {
		$this->cache = new ObjectCache($this->cache_domain);
	}

	function tearDown() {
		$this->cache->clear();
	}

	function ObjectCacheTest() {
		$this->UnitTestCase('ObjectCache test');
	}

	function testDomainPath() {
		$this->assertEqual($this->cache->getCacheDir(),  BASEPATH . '/tmp/cache/' . $this->cache_domain . '/');
	}

	function testNotExistingObject() {
		$this->assertFalse($this->cache->exists($this->object_id));
	}

	function testExistingObject() {
		$this->cache->save($this->object_id, $this->object_value);
		$this->assertTrue($this->cache->exists($this->object_id));
	}

	function testSavingAndLoading() {
		$this->cache->save($this->object_id, $this->object_value);
		$this->assertEqual($this->object_value, $this->cache->load($this->object_id));
	}
}
?>