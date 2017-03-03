<?php

use PP\Lib\PersistentQueue;
use PP\Cron\AbstractCron;

/**
 * Class PXCronRunQueue
 */
class PXCronRunQueue extends AbstractCron {

	/** @var string */
	public $name = 'Queue worker';

	/**
	 * @var int
	 */
	const MAX_JOBS_PER_RUN = 10;

	/**
	 * @param PXApplication $app
	 * @param PXDatabase $db
	 * @param PP\Lib\Datastruct\Tree $tree
	 * @param mixed $matchedTime
	 * @param PXCronRule $matchedRule
	 * @return array
	 */
	public function run(&$app, &$db, &$tree, $matchedTime, $matchedRule) {
		$queue = new PersistentQueue\Queue($app, $db);
		$jobs = $queue->getFreshJobs(static::MAX_JOBS_PER_RUN);
		$processedIds = [];

		foreach ($jobs as $job) {
			$jobId = $job->getId();
			$payload = $job->getPayload();

			$db->transactionBegin();

			$queue->startJob($job);
			$worker = $job->getWorker();

			try {
				$this->log(sprintf('Processing job #%d with %s', $jobId, get_class($worker)));

				$worker->run($payload);
				$queue->finishJob($job);
				$db->transactionCommit();

				$processedIds[] = $jobId;

				$this->log(Label('Successful with job: ' . $jobId));
			} catch (Exception $ex) {
				$db->transactionRollback();
				$queue->failJob($job);

				$this->log('Failed with job: ' . $jobId);
				$this->log($ex);
			}
		}

		return ['status' => 0, 'note' => 'Done with ' . join(', ', $processedIds)];
	}

}
