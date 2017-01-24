<?php

namespace PP\Command\Migrate;

use PP\Lib\Command\MigrateAbstractCommand;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Output\OutputInterface;


/**
 * Class MigrateListCommand
 * @package PP\Command
 */
class MigrateListCommand extends MigrateAbstractCommand {

	/**
	 * {@inheritdoc}
	 */
	protected function configure() {
		$this
			->setName('db:migrate:list')
			->setDescription('Display pending migrations');
	}

	/**
	 * {@inheritdoc}
	 */
	public function execute(InputInterface $input, OutputInterface $output) {
		$migrationList = $this->getPendingMigrations();
		if (count($migrationList) === 0) {
			$output->writeln("No pending migrations");
			return 0;
		}

		$output->writeln("Pending migrations list:");
		foreach ($migrationList as $migration) {
			$output->writeln("<info>${migration}</info>");
		}
	}
}
