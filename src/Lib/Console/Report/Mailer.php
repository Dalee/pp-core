<?php

namespace PP\Lib\Console\Report;

class Mailer {

	/** @var string */
	protected $mails = '';

	/** @var string */
	protected $from = 'example@example.com';

	/** @var string */
	protected $commandName = 'example:command';

	/** @var string */
	protected $projectName = 'example';

	/** @var array */
	protected $options = [];

	/**
	 * Set list of e-mails.
	 *
	 * @param string $mails - list of emails separated with comma
	 * @return $this
	 */
	public function setMails($mails) {
		$this->mails = $mails;

		return $this;
	}

	/**
	 * @return string
	 */
	public function getMails() {
		return $this->mails;
	}

	/**
	 * Sets e-mail field from.
	 *
	 * @param string $from
	 * @return $this
	 */
	public function setFrom($from) {
		$this->from = $from ? : $this->from;

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
	 */
	public function sendReport($data = '') {
		$mails = $this->getMails();
		if (!$mails) {
			return false;
		}

		$mail = new \NLMailMessage();
		$mail->setSubject("{$this->getProjectName()} PP {$this->getCommandName()} script results");
		$mail->setFrom("{$this->getProjectName()} PP", $this->getFrom());
		$mail->setBody($this->formatResultMailBody());

		if ($data) {
			$date = date('y:m:d_H:m:s');
			$fileName = sprintf("%s_%s_%s_report.txt", $date, $this->getProjectName(), $this->getCommandName());
			$fileName = str_replace(':', '-', $fileName);
			$mail->addFile($fileName, basename($fileName), 'text/plain', $data);
		}

		$mails = explode(',', $mails);
		$to = array_shift($mails);
		$cc = join(',', $mails);

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
		$body = [sprintf("%s PP %s script finished evaluation", $this->getProjectName(), $this->getCommandName())];

		if (!empty($this->getOptions())) {
			$body[] = '';
			$body[] = 'Used options:';
			$body[] = print_r($this->getOptions(), true);
		}

		return join(PHP_EOL, $body);
	}

}
