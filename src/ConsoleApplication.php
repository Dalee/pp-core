<?php

namespace PP;

use PXRegistry;

use PP\Properties\EnvLoader;
use Symfony\Component\Console\Application;
use Symfony\Component\Console\ConsoleEvents;
use Symfony\Component\Console\Input\InputOption;
use Symfony\Component\Console\Input\ArgvInput;
use Symfony\Component\Console\Input\InputInterface;
use PP\Lib\Console\Output\BuferringOutput;
use Symfony\Component\Console\Event\ConsoleEvent;
use Symfony\Component\Console\Event\ConsoleCommandEvent;
use Symfony\Component\Console\Event\ConsoleTerminateEvent;
use Symfony\Component\Console\Output\OutputInterface;
use Symfony\Component\EventDispatcher\EventDispatcher;
use Symfony\Component\Yaml\Yaml;
use Symfony\Component\DependencyInjection\ContainerAwareTrait;

use PP\Command\GetPropertyCommand;
use PP\Lib\Command\AbstractCommand;
use PP\Lib\Command\MigrateAbstractCommand;
use PP\Lib\Console\Report\Mailer;
use PP\Command\CronCommand;
use PP\Command\FillMetaCommand;
use PP\Command\FillUuidCommand;
use PP\Command\SetPropertyCommand;
use PP\Command\Migrate\MigrateListCommand;
use PP\Command\Migrate\MigrateUpCommand;
use PP\Command\Migrate\MigrateCreateCommand;

/**
 * Class ConsoleApplication.
 *
 * @package PP
 */
class ConsoleApplication extends Application {

	use ContainerAwareTrait;

	/** Proxima application instance */
	protected $app;

	/**
	 * @inheritdoc
	 */
	protected function getDefaultInputDefinition() {
		$definition = parent::getDefaultInputDefinition();

		$definition->addOptions([
			new InputOption('mail', 'm', InputOption::VALUE_IS_ARRAY | InputOption::VALUE_OPTIONAL,
				'Email addresses for report. For example, --mail=mail@domain.com --mail=test@test.ru'),
			new InputOption('send-report', 'S', InputOption::VALUE_NONE, 'Send report after command execution')
		]);

		return $definition;
	}

	public static function start() {
		$app = new static('pp', PP_VERSION);

		// set command loader
		$dispatcher = new EventDispatcher();
		$dispatcher->addListener(ConsoleEvents::COMMAND, function (ConsoleCommandEvent $event) use ($app) {
			$app->consoleCommandHandler($event);
		});

		$dispatcher->addListener(ConsoleEvents::TERMINATE, function (ConsoleTerminateEvent $event) use ($app) {
			$app->consoleTerminateHandler($event);
		});

		// create and run command set..
		$app->setDispatcher($dispatcher);
		$app->registerCoreCommands();
		$app->registerProjectCommands();
		$app->run(new ArgvInput(), new BuferringOutput());
	}

	/**
	 * Handles console command event of application.
	 *
	 * @param ConsoleCommandEvent $event
	 */
	protected function consoleCommandHandler(ConsoleCommandEvent $event) {
		$cmd = $event->getCommand();

		if ($cmd instanceof AbstractCommand) {
			$this->commonCommandHandler($event);
		}

		if ($cmd instanceof MigrateAbstractCommand) {
			$this->migrateCommandHandler($event);
		}
	}

	/**
	 * Configures common command before executing.
	 *
	 * @param ConsoleCommandEvent $event
	 */
	protected function commonCommandHandler(ConsoleCommandEvent $event) {
		$cmd = $event->getCommand();

		$engine = (new \PXEngineSbin())->start();
		$this->app = PXRegistry::getApp();
		$cmd->setContainer($engine->getContainer());
		$cmd->setApp($this->app)
			->setDb(PXRegistry::getDb());

		$address = $this->getTo($event->getInput());
		if ($this->isReporting($event) && !empty($address)) {
			$text = [
				'<info>After command execution auto-report will be sent to next e-mails:</info>',
				$address,
				''
			];
		} else {
			$text = [
				'<comment>Auto-report will not be sent after command execution</comment>',
				empty($address) ? 'No e-mail addresses' : '',
				''
			];
		}
		$event->getOutput()->writeln($text, OutputInterface::VERBOSITY_QUIET);
	}

	/**
	 * Configures migration-type command before execution.
	 *
	 * @param ConsoleCommandEvent $event
	 */
	protected function migrateCommandHandler(ConsoleCommandEvent $event) {
		$cmd = $event->getCommand();

		EnvLoader::inject();
		$dbDescription = \NLDBDescription::fromEnv();
		$cmd->setDbDriver($dbDescription->getDriver());
	}

	/**
	 * Handles console terminate event of application.
	 * Collects command output and sends email report.
	 *
	 * @param ConsoleTerminateEvent $event
	 * @throws \Exception
	 */
	protected function consoleTerminateHandler(ConsoleTerminateEvent $event) {
		if (!$this->isReporting($event)) {
			return;
		}

		$cmd = $event->getCommand();
		$input = $event->getInput();
		$output = $event->getOutput();

		$project = $this->app->getProperty('SYS_PROJECT_NAME', '');
		$from = $this->app->getProperty('SYS_COMMAND_REPORT_FROM', '');

		$reporter = new Mailer();
		$reporter->setCommandName($cmd->getName())
			->setOptions($input->getOptions())
			->setProjectName($project)
			->setFrom($from)
			->setTo($this->getTo($input))
			->sendReport($output->fetch());
	}

	/**
	 * Checks if reporting is available and enabled.
	 *
	 * @param ConsoleEvent $event
	 * @return bool
	 */
	protected function isReporting(ConsoleEvent $event) {
		$cmd = $event->getCommand();
		$input = $event->getInput();

		return $cmd instanceof AbstractCommand
			&& $input->getOption('send-report');
	}

	/**
	 * Gets mail list from options, env variable and property.
	 *
	 * @param InputInterface $input
	 * @return string
	 */
	protected function getTo(InputInterface $input) {
		$addresses = join(',', $input->getOption('mail'))
			?: EnvLoader::get('PP_COMMAND_REPORT_MAIL')
			?: $this->app->getProperty('SYS_COMMAND_REPORT_MAIL', '');

		return $addresses;
	}

	/**
	 * Register bundled commands. Those commands should be available
	 * to every project.
	 *
	 * @return $this
	 */
	protected function registerCoreCommands() {
		$this->add(new CronCommand());
		$this->add(new GetPropertyCommand());
		$this->add(new SetPropertyCommand());
		$this->add(new FillMetaCommand());
		$this->add(new FillUuidCommand());
		$this->add(new MigrateListCommand());
		$this->add(new MigrateUpCommand());
		$this->add(new MigrateCreateCommand());

		return $this;
	}

	/**
	 * Load project commands (if exists).
	 *
	 * @return $this
	 * @throws \Exception
	 */
	protected function registerProjectCommands() {
		$filePath = BASEPATH . '/app/config/commands.yml';
		$filePath = path_clear($filePath);

		if (file_exists($filePath)) {
			$result = Yaml::parse(file_get_contents($filePath));
			$commandList = isset($result['commands']) && is_array($result['commands'])
				? $result['commands']
				: [];

			foreach ($commandList as $className) {
				if (!class_exists($className)) {
					throw new \Exception("Command class '${className}' doesn't exist");
				}

				$this->add(new $className);
			}
		}

		return $this;
	}

}
