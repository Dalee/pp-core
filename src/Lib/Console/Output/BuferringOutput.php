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

		$cleanMessages = array_map('strip_tags', $messages);
		$sep = $newline ? "\n" : '';

		$this->buffer .= join($sep, $cleanMessages) . $sep;

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
