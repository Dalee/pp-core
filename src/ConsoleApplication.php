<?php

namespace PP;

use PP\Lib\Command\AbstractCommand;
use Symfony\Component\Console\Application;
use Symfony\Component\Console\ConsoleEvents;
use Symfony\Component\Console\Event\ConsoleCommandEvent;
use Symfony\Component\EventDispatcher\EventDispatcher;
use Symfony\Component\Yaml\Yaml;

class ConsoleApplication extends Application {

	public static function start() {
		// set dispatcher
		$dispatcher = new EventDispatcher();
		$dispatcher->addListener(ConsoleEvents::COMMAND, function (ConsoleCommandEvent $event) {
			$cmd = $event->getCommand();

			// if command is belongs to pp/core, it should be instance of ConsoleCommand
			if ($cmd instanceof AbstractCommand) {
				(new \PXEngineSbin())->start();
				$cmd
					->setApp(\PXRegistry::getApp())
					->setDb(\PXRegistry::getDb());
			}
		});

		// create and run command set..
		$app = new static('pp', PP_VERSION);
		$app->setDispatcher($dispatcher);
		$app->loadCommands();
		$app->run();
	}

	/**
	 *
	 */
	protected function loadCommands() {
		$filePath = BASEPATH . '/app/config/commands.yml';
		$filePath = path_clear($filePath);

		if (file_exists($filePath)) {
			$result = Yaml::parse(file_get_contents($filePath));
			if (isset($result['commands']) && is_array($result['commands'])) {
				foreach ($result['commands'] as $className) {
					$this->add(new $className);
				}
			}
		}
	}
}
