<?php

namespace PP;

use PP\Properties\EnvLoader;
use Symfony\Component\Console\Application;
use Symfony\Component\Console\ConsoleEvents;
use Symfony\Component\Console\Event\ConsoleCommandEvent;
use Symfony\Component\EventDispatcher\EventDispatcher;
use Symfony\Component\Yaml\Yaml;
use Symfony\Component\DependencyInjection\ContainerAwareInterface;
use Symfony\Component\DependencyInjection\ContainerAwareTrait;

use PP\Command\GetPropertyCommand;
use PP\Lib\Command\AbstractCommand;
use PP\Lib\Command\MigrateAbstractCommand;
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

	public static function start() {
		// set command loader
		$dispatcher = new EventDispatcher();
		$dispatcher->addListener(ConsoleEvents::COMMAND, function (ConsoleCommandEvent $event) {
			$cmd = $event->getCommand();

			if ($cmd instanceof AbstractCommand) {
				$engine = (new \PXEngineSbin())->start();
				$cmd->setContainer($engine->getContainer());
				$cmd->setApp(\PXRegistry::getApp())
					->setDb(\PXRegistry::getDb());
			}

			if ($cmd instanceof MigrateAbstractCommand) {
				$dbDescription = \NLDBDescription::fromEnv();
				$cmd->setDbDriver($dbDescription->getDriver());
			}

		});

		// create and run command set..
		$app = new static('pp', PP_VERSION);
		$app->setDispatcher($dispatcher);
		$app->registerCoreCommands();
		$app->registerProjectCommands();
		$app->run();
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
