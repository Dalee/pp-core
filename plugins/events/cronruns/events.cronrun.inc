<?php

/*
 * Обработка региональных объектов
 * Удаление, добавление региона.
 */

class PXCronRunEvents extends PXAbstractCronRun {
	var $name;
	const SUCCESS_STATE = 'ok';

	function __construct() {
		$this->name = 'Обработка отложенных событий';
	}


	function Run($app, $db, $tree, $matchedTime, $matchedRule) {
		$this->app = $app;
		$db->Close();

		while (1) {
			PXRegistry::getDb()->Connect();
			$event = PXEventsQueue::dequeue();
			if (is_null($event)) break;
			PXRegistry::getDb()->Close();
			$this->dispatch($event);
		}

		return 'ok';
	}

	function dispatch($event) {
		if (empty($event['callback']) || empty($event['params'])) {
			return;
		}

		extract($event['callback']);
		$resultState = -1;

		if (is_callable(array($class, $method))) {
			Label("Start event ${class}::${method}");

			switch ($pid = pcntl_fork()) {
				case -1:
					break;

				case 0:
					PXRegistry::getDb()->Connect();
					$resultState = call_user_func_array(array($class, $method), array($event['params']));
					$this->write_audit($class, $method, $event, $resultState);
					break;

				default:
					pcntl_waitpid($pid, $status);

					// php crash in callback process
					if ($status) {
						$resultState = 'fatal';
					}

					break;
			}
		}
	}

	function write_audit($class, $method, $event, $resultState) {
		$logMethod = $resultState == self::SUCCESS_STATE ?  'info' : 'error';
		$audit = PXAuditLogger::getLogger(PXRegistry::getApp(), PXRegistry::getDB());
		$audit->$logMethod(join('/', array($class, $method)), "events/{$event['id']}");
	}
}

?>
