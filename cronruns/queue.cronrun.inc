<?php

use PP\Lib\PersistentQueue;
use PP\Cron\AbstractCron;

/**
 * Class PXCronRunQueue
 */
class PXCronRunQueue extends AbstractCron {

	/**
	 * @var string
	 */
	public $name = 'Queue worker';

	/**
	 * @var int
	 */
	const MAX_JOBS_PER_RUN = 10;

	/**
	 * @return string
	 */
	protected function getMailTemplatePath() {
		return PPPATH . '/mail_templates/job_notification.tmpl';
	}

	/**
	 * @param ArrayIterator $results
	 * @return string
	 */
	protected function formatResults(ArrayIterator $results) {
		return join('<br />', $results->getArrayCopy());
	}

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

				$worker->setJob($job)->run($payload);
				$queue->finishJob($job);
				$db->transactionCommit();
				$subject = 'Successful Job #' . $jobId;

				$processedIds[] = $jobId;

				$this->log(Label('Successful with job: ' . $jobId));
			} catch (Exception $ex) {
				$db->transactionRollback();
				$job->getResultBag()->addError(nl2br($ex->getMessage()));
				$queue->failJob($job);
				$subject = 'Failed Job #' . $jobId;

				$this->log('Failed with job: ' . $jobId);
				$this->log($ex);
			}

			$owner = $db->getObjectById($app->types['suser'], $job->getOwnerId());
			$toEmail = getFromArray($owner, 'email');
			$toName = getFromArray($owner, 'title');
			$resultBag = $job->getResultBag();

			if ($toName && isEmail($toEmail)) {
				$message = new NLMailMessage();
				$message->setFormat('html');
				$message->setTemplate($this->getMailTemplatePath(), true, [
					'subject' => $subject,
					'id' => $jobId,
					'errors' => $this->formatResults($resultBag->getErrors()),
					'info' => $this->formatResults($resultBag->getInfo()),
					'notices' => $this->formatResults($resultBag->getNotices())
				]);
				$message->setTo($toName, $toEmail);
				if (!$message->send()) {
					$this->log(sprintf('Failed to notify %s on job #%d.', $toEmail, $jobId));
				}
			}
		}

		return ['status' => 0, 'note' => 'Done with ' . join(', ', $processedIds)];
	}

}
