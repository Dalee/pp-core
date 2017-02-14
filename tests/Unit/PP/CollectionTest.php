<?php

namespace Tests\Unit\PP;

use PP\Lib\Collection;
use Tests\Base\AbstractUnitTest;

class CollectionTest extends AbstractUnitTest {

	public function testLast() {
		$collection = new Collection();
		$collection
			->push('first')
			->push('second')
			->push('third');
		$last = $collection->last();

		$this->assertEquals('third', $last);
	}

	public function testJsonSerializeEqualsToArray() {
		$collection = new Collection();
		$collection->push(1)->push(2);

		$this->assertJsonStringEqualsJsonString(
			json_encode($collection->toArray()),
			json_encode($collection)
		);
	}

}
