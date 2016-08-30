<?php

class PXCronRunNull extends PXAbstractCronRun {
	public $name;

	public function __construct() {
		$this->name = 'Пустая задача';
	}

	public function Run(&$app, &$db, &$tree, $matchedTime, $matchedRule) {
		return array('status' => 0, 'note' => 'Успешно!');
	}
}
