<?php

namespace PP\Command;

use PP\Lib\Command\AbstractCommand;
use Symfony\Component\Console\Input\InputArgument;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Output\ConsoleOutputInterface;
use Symfony\Component\Console\Output\OutputInterface;

/**
 * Get property value.
 * Usable in bash scripting:
 * 	- stdout - value, or empty string
 * 	- stderr - error text
 *  - exit_status - 0, everything is ok, 1 - property not found, > 1 - other error happened
 *
 * Best usage as:
 * `pp db:get-property -- "ENVIRONMENT"`
 *
 * Class SetProperty
 * @package PP\Command
 */
class GetPropertyCommand extends AbstractCommand {

	/**
	 * {@inheritdoc}
	 */
	protected function configure() {
		$this
			->setName('db:property:get')
			->setDescription('Get application property')
			->setHelp('Get property')
			->addArgument('key', InputArgument::REQUIRED, 'property name');
	}

	/**
	 * {@inheritdoc}
	 */
	protected function execute(InputInterface $input, OutputInterface $output) {
		$key = $input->getArgument('key');
		$stderr = null;

		// in case no tty present..
		if ($output instanceof ConsoleOutputInterface) {
			$stderr = $output->getErrorOutput();
		}

		if (empty($key)) {
			if ($stderr instanceof OutputInterface) {
				$stderr->writeln("Empty key passed");
				return 2;
			}
		}

		$sql = sprintf('SELECT "value" FROM %s WHERE "name"=\'%s\'', DT_PROPERTIES, $this->db->EscapeString($key));
		$result = $this->db->query($sql);

		if (count($result) === 0) {
			if ($stderr instanceof OutputInterface) {
				$stderr->writeln("Property: ${key} not found");
			}
			return 1;
		}

		$result = array_shift($result);
		$output->write($result['value']);
		return 0;
	}
}
