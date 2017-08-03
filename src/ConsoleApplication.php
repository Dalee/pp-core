<?php

namespace PP;

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
use Symfony\Component\DependencyInjection\ContainerAwareInterface;
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
		// set command loader
		$dispatcher = new EventDispatcher();
		$dispatcher->addListener(ConsoleEvents::COMMAND, function (ConsoleCommandEvent $event) {
			static::consoleCommandHandler($event);
		});

		$dispatcher->addListener(ConsoleEvents::TERMINATE, function (ConsoleTerminateEvent $event) {
			static::consoleTerminateHandler($event);
		});

		// create and run command set..
		$app = new static('pp', PP_VERSION);
		$app->setDispatcher($dispatcher);
		$app->registerCoreCommands();
		$app->registerProjectCommands();
		$app->run(new ArgvInput(), new BuferringOutput());
	}

	/**
	 * Handles console command event of application.
	 * Configures command before executing.
	 *
	 * @param ConsoleCommandEvent $event
	 */
	protected static function consoleCommandHandler(ConsoleCommandEvent $event) {
		$cmd = $event->getCommand();

		if ($cmd instanceof AbstractCommand) {
			$engine = (new \PXEngineSbin())->start();
			$cmd->setContainer($engine->getContainer());
			$cmd->setApp(\PXRegistry::getApp())
				->setDb(\PXRegistry::getDb());

			if (static::isReporting($event)) {
				$text = [
					'<info>After command execution auto-report will be sent to next mails:</info>',
					static::getMails($event->getInput()),
					''
				];
			} else {
				$text = ['<comment>Auto-report will not be sent after command execution</comment>', ''];
			}
			$event->getOutput()->writeln($text, OutputInterface::VERBOSITY_QUIET);
		}

		if ($cmd instanceof MigrateAbstractCommand) {
			$dbDescription = \NLDBDescription::fromEnv();
			$cmd->setDbDriver($dbDescription->getDriver());
		}
	}

	/**
	 * Handles console terminate event of application.
	 * Collects command output and sends email report.
	 *
	 * @param ConsoleTerminateEvent $event
	 */
	protected static function consoleTerminateHandler(ConsoleTerminateEvent $event) {
		$cmd = $event->getCommand();
		$input = $event->getInput();
		$output = $event->getOutput();

		if (!static::isReporting($event)) {
			return;
		}

		$reporter = new Mailer();
		$reporter->setCommandName($cmd->getName())
			->setOptions($input->getOptions())
			->setProjectName(\PXRegistry::getApp()->getProperty('SYS_PROJECT_NAME', ''))
			->setFrom(\PXRegistry::getApp()->getProperty('SYS_COMMAND_REPORT_FROM', ''))
			->setMails(static::getMails($input))
			->sendReport($output->fetch());
	}

	/**
	 * Checks if reporting is available and enabled.
	 *
	 * @param ConsoleEvent $event
	 * @return bool
	 */
	protected static function isReporting(ConsoleEvent $event) {
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
	protected static function getMails(InputInterface $input) {
		$mails = join(',', $input->getOption('mail'));
		if (empty($mails)) {
			$mails = EnvLoader::get('PP_COMMAND_REPORT_MAIL');
			if (empty($mails)) {
				$mails = \PXRegistry::getApp()->getProperty('SYS_COMMAND_REPORT_MAIL', '');
			}
		}

		return $mails;
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
