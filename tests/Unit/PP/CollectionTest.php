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

}
