<?php

namespace PP\Command;

use Symfony\Component\Console\Command\Command;
use PP\DependencyInjection\ContainerAwareInterface;
use Symfony\Component\Console\Input\InputArgument;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Input\InputOption;
use Symfony\Component\Console\Output\OutputInterface;
use PP\Lib\Command\AbstractCommand;

/**
 * Cron command
 *
 * Class CronCommand
 * @package PP\Command
 */
class CronCommand extends AbstractCommand
{
    /**
     * {@inheritdoc}
     */
    protected function configure()
    {
        $this
            ->setName('cron')
            ->setDescription('Run scheduled cron jobs')
            ->addOption('list', 'l', InputOption::VALUE_NONE, 'display list of cronruns')
            ->addArgument('task', InputArgument::OPTIONAL, 'task name to run');
    }

    /**
     * {@inheritdoc}
     */
    public function execute(InputInterface $input, OutputInterface $output): int
    {
        if (!isset($this->app->modules['cronrun'])) {
            return Command::SUCCESS;
        }

        /** @var \PP\Module\CronRunModule $cronModule */
        $cronModule = $this->app->modules['cronrun']->getModule();

        if ($cronModule instanceof ContainerAwareInterface) {
            $cronModule->setContainer($this->container);
        }

        $listTasks = $input->getOption('list');
        $task = $input->getArgument('task');

        if ($listTasks === true) {
            $output->writeln('<info>'.str_repeat("-", 132).'</info>');
            $header = [
                mb_str_pad('Название задачи', 25),
                mb_str_pad('Расписание', 15),
                mb_str_pad('Описание задачи', 40),
                mb_str_pad('Дата запуска', 21),
                mb_str_pad('Дата завершения', 21),
            ];

            $output->writeln('<info>' . join(' | ', $header) . '</info>');
            $output->writeln('<info>' . str_repeat('-', 132) . '</info>');

            foreach ($cronModule->jobs as $task => $j) {
                $stat = $cronModule->getStat($j);

                $title = mb_str_pad($task, 25);
                $title = (mb_strlen((string) $title) > 25)
                    ? mb_substr((string) $title, 0, 22) . '...'
                    : $title;

                $description = mb_str_pad($j['job']->name, 40);
                $description = (mb_strlen((string) $description) > 40)
                    ? mb_substr((string) $description, 0, 37).'...'
                    : $description;

                $row = [
                    '<comment>' . $title . '</comment>',
                    mb_str_pad($j['rule']->asString, 15),
                    $description,
                    mb_str_pad(strftime("%Y-%m-%d %H:%M:%S", $stat['start'] ?? null), 21),
                    mb_str_pad(strftime("%Y-%m-%d %H:%M:%S", $stat['end'] ?? null), 21),
                ];

                $output->writeln(implode(' | ', $row));
            }

            $output->writeln('');

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

        return Command::SUCCESS;
    }
}
