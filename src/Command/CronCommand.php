<?php

namespace PP\Command;

use PP\Lib\Command\AbstractCommand;
use Symfony\Component\Console\Input\InputArgument;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Input\InputOption;
use Symfony\Component\Console\Output\OutputInterface;
use Symfony\Component\Console\Style\SymfonyStyle;

/**
 * Cron command
 *
 * Class CronCommand
 * @package PP\Command
 */
class CronCommand extends AbstractCommand {

	/**
	 * {@inheritdoc}
	 */
	protected function configure() {
		$this
			->setName('cron')
			->setDescription('Run scheduled cron jobs')
			->addOption('list', 'l', InputOption::VALUE_NONE, 'display list of cronruns')
			->addArgument('task', InputArgument::OPTIONAL, 'task name to run');
	}

	/**
	 * {@inheritdoc}
	 */
	public function run(InputInterface $input, OutputInterface $output) {

		if (!isset($this->app->modules['cronrun'])) {
			return;
		}

		/** @var \PXModuleCronRun $cronModule */
		$cronModule = $this->app->modules['cronrun']->getModule();

		$listTasks = $input->getOption('list');
		$task = $input->getArgument('task');

		if ($listTasks === true) {
			$output->writeln('<info>'.str_repeat("-", 128).'</info>');
			$header = [
				mb_str_pad('Название задачи', 20),
				mb_str_pad('Расписание', 15),
				mb_str_pad('Описание задачи', 40),
				mb_str_pad('Дата запуска', 21),
				mb_str_pad('Дата завершения', 21),
			];
			$output->writeln('<info>'.implode(' | ', $header).'</info>');
			$output->writeln(str_repeat("-", 128));

			foreach ($cronModule->jobs as $task => $j) {
				$stat = $cronModule->getStat($j);
				$row = [
					mb_str_pad($task, 20),
					mb_str_pad($j['rule']->asString, 15),
					mb_str_pad($j['job']->name, 40),
					mb_str_pad(strftime("%Y-%m-%d %H:%M:%S", $stat['start']), 21),
					mb_str_pad(strftime("%Y-%m-%d %H:%M:%S", $stat['end']), 21),
				];

				$output->writeln(implode(' | ', $row));
			}
			$output->writeln(str_repeat("-", 128));

		} elseif ($task !== null) {
			if (!isset($cronModule->jobs[$task])) {
				throw new \InvalidArgumentException("Unknown task: {$task}");
			}

			$output->writeln('<info>Starting requested job:</info> ' . $task);
			$cronModule->runJob($cronModule->jobs[$task], $this->app, time());

		} else {
			$output->writeln('<info>Starting scheduled jobs..</info>');
			$cronModule->RunTasks($this->app, time());
		}
	}
}
