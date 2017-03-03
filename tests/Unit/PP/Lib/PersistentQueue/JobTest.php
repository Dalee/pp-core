<?php

namespace Tests\Unit\PP;

use Tests\Base\AbstractUnitTest;
use PP\Lib\PersistentQueue\Job;

class JobTest extends AbstractUnitTest {

	public function testInitialStateFresh() {
		$job = new Job();
		$this->assertEquals(Job::STATE_FRESH, $job->getState());
	}

	//

}
