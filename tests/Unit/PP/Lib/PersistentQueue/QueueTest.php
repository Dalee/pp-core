<?php

namespace Tests\Unit\PP\Lib\PersistentQueue;

use PHPUnit\Framework\MockObject\MockObject;
use PP\Lib\Config\ApplicationInterface;
use PP\Lib\PersistentQueue\Job;
use PP\Lib\PersistentQueue\Queue;
use Tests\Base\AbstractUnitTest;

class QueueTest extends AbstractUnitTest {

	protected Queue $queue;

	protected \PXDatabase|MockObject $db;

	protected ApplicationInterface|MockObject $app;

	/**
	 * @before
	 */
	public function before() {
		$this->app = $this->getMockBuilder(\PXApplication::class)
			->disableOriginalConstructor()
			->onlyMethods(['initContentObject'])
			->getMock();

		$this->app->types = [
			Queue::JOB_DB_TYPE => new \PXTypeDescription()
		];

		$this->db = $this->getMockBuilder(\PXDatabase::class)
			->disableOriginalConstructor()
			->onlyMethods(['addContentObject', 'modifyContentObject', 'getObjectsByFieldLimited'])
			->getMock();

		$this->queue = new Queue($this->app, $this->db);
	}

	public function testStartJob() {
		$job = new Job();
		$expected = (new Job())->setState(Job::STATE_IN_PROGRESS);

		$this->db->expects($this->once())
			->method('modifyContentObject')
			->with(
				$this->equalTo($this->app->getDataType(Queue::JOB_DB_TYPE)),
				$this->equalTo($expected->toArray())
			);

		$this->queue->startJob($job);
	}

	public function testFailJob() {
		$job = new Job();
		$expected = (new Job())->setState(Job::STATE_FAILED);

		$this->db->expects($this->once())
			->method('modifyContentObject')
			->with(
				$this->equalTo($this->app->getDataType(Queue::JOB_DB_TYPE)),
				$this->equalTo($expected->toArray())
			);

		$this->queue->failJob($job);
	}

	public function testFinishJob() {
		$job = new Job();
		$expected = (new Job())->setState(Job::STATE_FINISHED);

		$this->db->expects($this->once())
			->method('modifyContentObject')
			->with(
				$this->equalTo($this->app->getDataType(Queue::JOB_DB_TYPE)),
				$this->equalTo($expected->toArray())
			);

		$this->queue->finishJob($job);
	}

	public function testGetFreshJobs() {
		$limit = 2;
		$jobOne = (new Job())
			->setPayload(['test_id' => 1])
			->setId(1)
			->setWorker($this->getMockForAbstractClass(\PP\Lib\PersistentQueue\WorkerInterface::class, []));

		$jobTwo = (new Job())
			->setPayload(['test_id' => 2])
			->setId(2)
			->setWorker($this->getMockForAbstractClass(\PP\Lib\PersistentQueue\WorkerInterface::class, []));

		$this->db->expects($this->once())
			->method('getObjectsByFieldLimited')
			->with(
				$this->equalTo($this->app->getDataType(Queue::JOB_DB_TYPE)),
				$this->equalTo(true),
				$this->equalTo('state'),
				$this->equalTo(Job::STATE_FRESH),
				$this->equalTo($limit),
				$this->equalTo(0)
			)
			->willReturn([$jobOne->toArray(), $jobTwo->toArray()]);

		$jobs = $this->queue->getFreshJobs($limit);

		$this->assertEquals([$jobOne, $jobTwo], $jobs);
	}

	public function testAddJob() {
		$job = (new Job())
			->setWorker($this->getMockForAbstractClass(\PP\Lib\PersistentQueue\WorkerInterface::class, []));

		$stub = ['stub_param' => 'stub_value'];

		$this->app->expects($this->once())
			->method('initContentObject')
			->with($this->equalTo(Queue::JOB_DB_TYPE))
			->willReturn($stub);

		$this->db->expects($this->once())
			->method('addContentObject')
			->with(
				$this->equalTo($this->app->getDataType(Queue::JOB_DB_TYPE)),
				array_merge($stub, $job->toArray())
			)
			->willReturn(1);

		$id = $this->queue->addJob($job);

		$this->assertGreaterThan(0, $id);
	}

}
