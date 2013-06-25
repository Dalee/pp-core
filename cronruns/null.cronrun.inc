<?php
class PXCronRunNull extends PXAbstractCronRun {
	var $name;

	function PXCronRunNull() {
		$this->name = 'Пустая задача';
	}

	function Run(&$app, &$db, &$tree, $matchedTime, $matchedRule) {
		return array('status' => 0, 'note' => 'Успешно!');
	}

}
?>