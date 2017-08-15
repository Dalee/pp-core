<?php

namespace PP\Lib\Console\Report;

use Exception;
use NLMailMessage;

/**
 * Class Mailer
 * @package PP\Lib\Console\Report
 */
class Mailer {

	/** @var string */
	protected $to = '';

	/** @var string */
	protected $from = '';

	/** @var string */
	protected $commandName = 'example:command';

	/** @var string */
	protected $projectName = 'example';

	/** @var array */
	protected $options = [];

	/**
	 * Set list of e-mails.
	 *
	 * @param string $to - list of emails separated with comma
	 * @return $this
	 */
	public function setTo($to) {
		$this->to = $to;

		return $this;
	}

	/**
	 * @return string
	 */
	public function getTo() {
		return $this->to;
	}

	/**
	 * Sets e-mail field from.
	 *
	 * @param string $from
	 * @return $this
	 */
	public function setFrom($from) {
		$this->from = $from;

		return $this;
	}

	/**
	 * @return string
	 */
	public function getFrom() {
		return $this->from;
	}

	/**
	 * @param string $name
	 * @return $this
	 */
	public function setCommandName($name) {
		$this->commandName = $name;

		return $this;
	}

	/**
	 * @return string
	 */
	public function getCommandName() {
		return $this->commandName;
	}

	/**
	 * @param string $name
	 * @return $this
	 */
	public function setProjectName($name) {
		$this->projectName = $name;

		return $this;
	}

	/**
	 * @return string
	 */
	public function getProjectName() {
		return $this->projectName;
	}

	/**
	 * @param array $options
	 * @return $this
	 */
	public function setOptions($options) {
		$this->options = $options;

		return $this;
	}

	/**
	 * @return array
	 */
	public function getOptions() {
		return $this->options;
	}

	/**
	 * Sends result to mail addresses.
	 *
	 * @param string $data
	 * @return bool
	 * @throws Exception
	 */
	public function sendReport($data = '') {
		$address = $this->getTo();
		if (!$address) {
			throw new Exception('Empty email list.');
		}

		if (!$this->getFrom()) {
			throw new Exception('Empty from field.');
		}

		$project = $this->getProjectName();
		$command = $this->getCommandName();

		$mail = new NLMailMessage();
		$mail->setSubject(sprintf('%s PP %s script results', $project, $command));
		$mail->setFrom(sprintf('%s PP', $project), $this->getFrom());
		$mail->setBody($this->formatResultMailBody());

		if ($data) {
			$date = date('y:m:d_H:m:s');
			$fileName = sprintf('%s_%s_%s_report.txt', $date, $project, $command);
			$fileName = str_replace(':', '-', $fileName);
			$mail->addFile($fileName, basename($fileName), 'text/plain', $data);
		}

		$addresses = explode(',', $address);
		$to = array_shift($addresses);
		$cc = join(',', $addresses);

		if ($cc) {
			$mail->setCC($cc, false);
		}

		$mail->setTo($to, $to);
		return $mail->send();
	}

	/**
	 * Formats usual result mail body.
	 *
	 * @return string
	 */
	public function formatResultMailBody() {
		$finishFormat = '%s PP %s script finished evaluation';
		$body = [
			sprintf($finishFormat, $this->getProjectName(), $this->getCommandName()),
			'',
			'Used options:',
			print_r($this->getOptions(), true)
		];

		return join(PHP_EOL, $body);
	}

}
