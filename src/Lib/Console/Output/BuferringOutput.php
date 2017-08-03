<?php

namespace PP\Lib\Console\Output;

use Symfony\Component\Console\Output\ConsoleOutput;

/**
 * Class ConsoleFileOutput
 * @package PP\Lib\Output
 */
class BuferringOutput extends ConsoleOutput {

	/** @var string */
	protected $buffer = '';

	/**
	 * @inheritdoc
	 */
	public function write($messages, $newline = false, $options = self::OUTPUT_NORMAL) {
		$messages = (array) $messages;

		foreach ($messages as $message) {
			$msg = strip_tags($message);
			$msg = $newline ? $msg . PHP_EOL : $msg;
			$this->buffer .= $msg;
		}

		parent::write($messages, $newline, $options);
	}

	/**
	 * Fetches buffer and clears it.
	 *
	 * @return string
	 */
	public function fetch() {
		$result = $this->buffer;
		$this->buffer = '';

		return $result;
	}

}
