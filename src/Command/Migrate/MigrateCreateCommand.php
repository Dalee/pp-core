<?php

namespace PP\Command\Migrate;

use PP\Lib\Command\MigrateAbstractCommand;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Output\OutputInterface;

/**
 * Class MigrateCreateCommand
 * @package PP\Command
 */
class MigrateCreateCommand extends MigrateAbstractCommand {

	/**
	 * {@inheritdoc}
	 */
	protected function configure() {
		$this
			->setName('db:migrate:create')
			->setDescription('Create new migration');
	}

	/**
	 * {@inheritdoc}
	 */
	public function execute(InputInterface $input, OutputInterface $output) {
		$fileName = sprintf("%f", microtime(true));
		$fileName = str_replace(',', '', $fileName);
		$fileName = sprintf("%s.php", $fileName);

		$className = $this->getMigrationClass($fileName);
		$migrationContent = str_replace(
			['{{namespace}}', '{{class}}'],
			[$this->namespace, $className],
			$this->template
		);

		$directory = $this->getMigrationsDirectory();
		$writePath = join(DIRECTORY_SEPARATOR, [
			rtrim($directory, DIRECTORY_SEPARATOR),
			$fileName
		]);

		if (!is_writable($directory)) {
			throw new \Exception("Can't write to directory: ${directory}");
		}

		file_put_contents($writePath, $migrationContent);
		$output->writeln("<info>Migration created:</info> ${fileName}");
	}
}
