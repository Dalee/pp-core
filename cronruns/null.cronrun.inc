<?php

use PP\Cron\AbstractCron;

/**
 * Class PXCronRunNull.
 */
class PXCronRunNull extends AbstractCron {

	/**
	 * @var string
	 */
	public $name;

	/**
	 * PXCronRunNull constructor.
	 */
	public function __construct() {
		$this->name = 'Пустая задача';
	}

	public function Run(&$app, &$db, &$tree, $matchedTime, $matchedRule) {
		return ['status' => 0, 'note' => 'Успешно!'];
	}

}
