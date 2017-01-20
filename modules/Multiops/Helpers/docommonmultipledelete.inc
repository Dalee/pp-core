<?php

foreach ($this->helper->objectIds as $objectId) {
	$this->db->DeleteContentObject($this->app->types[$this->helper->objectType], $objectId);
}
$cnt = count($this->helper->objectIds);
$res = sprintf('%s %d %s', NumericEndingsRussian($cnt, 'удалён', 'удалено', 'удалено'), $cnt, NumericEndingsRussian($cnt, 'объект', 'объекта', 'объектов'));

$RESULT = PXEngineJSON::toSuccess($res);
