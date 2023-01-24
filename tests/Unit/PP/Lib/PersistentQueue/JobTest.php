<?php

namespace Tests\Unit\PP\Lib\PersistentQueue;

use \UnexpectedValueException;
use Tests\Base\AbstractUnitTest;
use PP\Lib\PersistentQueue\Job;

class JobTest extends AbstractUnitTest {

	public function testInitialStateFresh() {
		$job = new Job();
		$this->assertEquals(Job::STATE_FRESH, $job->getState());
	}

	public function testSetInvalidState() {
		$this->expectExceptionMessageMatches("/State is invalid/");
		$this->expectException(UnexpectedValueException::class);
		$job = new Job();
		$job->setState('INVALID STATE');
	}

	public function testFromArrayWorkerClassExists() {
		$this->expectExceptionMessageMatches("/Worker class does not exist/");
		$this->expectException(UnexpectedValueException::class);

		$arrayJob = ['worker' => 'PP\\SomeInexistentClass'];
		$job = Job::fromArray($arrayJob);
		$job->getWorker();
	}

	public function testFromArrayWorkerInvalidInterface() {
		$this->expectExceptionMessageMatches("/Worker class does not implement/");
		$this->expectException(UnexpectedValueException::class);
		$arrayJob = ['title' => \PP\Lib\PersistentQueue\Job::class];
		$job = Job::fromArray($arrayJob);
		$job->getWorker();
	}

	public function testFromArrayWorker() {
		$worker = $this->getMockForAbstractClass(\PP\Lib\PersistentQueue\WorkerInterface::class, []);
		$workerClass = $worker::class;
		$arrayJob = ['title' => $workerClass];
		$job = Job::fromArray($arrayJob);

		$this->assertEquals($worker, $job->getWorker());
	}

	public function testToArray() {
		$job = new Job();
		$worker = $this->getMockForAbstractClass(\PP\Lib\PersistentQueue\WorkerInterface::class, []);
		$job->setPayload(['test_key' => 'test_value']);
		$job->setWorker($worker);
		$job->setOwnerId(2);
		$actual = $job->toArray();

		$expected = [
			'id' => 0,
			'title' => $worker::class,
			'payload' => ['test_key' => 'test_value'],
			'state' => Job::STATE_FRESH,
			'sys_owner' => 2,
			'result' => [
				'errors' => [],
				'notices' => [],
				'info' => []
			]
		];
		$this->assertEquals($expected, $actual);
	}

}
