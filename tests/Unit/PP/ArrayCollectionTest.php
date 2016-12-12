<?php

namespace Tests\Unit\PP;

use PP\Lib\ArrayCollection;
use Tests\Base\AbstractUnitTest;

class ArrayCollectionTest extends AbstractUnitTest {

	public function testArraySet() {
		$collection = new ArrayCollection();
		$collection['hello'] = 'world';

		$this->assertEquals('world', $collection['hello']);
		$this->assertArrayHasKey('hello', $collection);
	}

	public function testArrayUnset() {
		$collection = new ArrayCollection();
		$collection['hello'] = 'world';

		$this->assertArrayHasKey('hello', $collection);
		unset($collection['hello']);
		$this->assertArrayNotHasKey('hello', $collection);
	}

	public function testArrayKeyExists() {
		$collection = new ArrayCollection();
		$collection['hello'] = 'world';

		$result = $collection->offsetExists('hello');
		$this->assertEquals(true, $result);
	}

	public function testFromArray() {
		$collection = new ArrayCollection();
		$collection->fromArray([
			'hello' => 'world'
		]);

		$this->assertArrayHasKey('hello', $collection);
		$this->assertEquals('world', $collection['hello']);
	}

	public function testGetByPath() {
		$collection = new ArrayCollection();
		$collection->fromArray([
			'hello' => [
				'world' => 'Earth',
				'we' => [
					'need' => 'go deeper'
				]
			]
		]);

		$result = $collection->getByPath('hello.world', '.');
		$this->assertEquals('Earth', $result);

		$result = $collection->getByPath('hello|world', '|');
		$this->assertEquals('Earth', $result);

		$result = $collection->getByPath('hello.we.need');
		$this->assertEquals('go deeper', $result);
	}
}
