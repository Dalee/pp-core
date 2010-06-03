<?php
class PXTriggerDatabaseSearch extends PXAbstractDatabaseTrigger {
	function OnAddObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues) {
		if (!isset($format->fields['index'])) return;
		if (!$object['index']) return;
		$this->writeExp('U', $objId, $format);
	}
 
	function OnDeleteObject(&$db, &$objId, &$format) {
		if (!isset($format->fields['index'])) return;
		$this->writeExp('D', $objId, $format);
	}
 
	function OnModifyObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues) {
		if (!isset($format->fields['index'])) return;
		if (!$object['index']) {
			$this->writeExp('D', $objId, $format);
		} else {
			$this->writeExp('U', $objId, $format);
		}
	}
 
	function writeExp($typeExp, &$objId, &$format) {
		$fp = fopen(BASEPATH."site/var/contentExpired.idx", "a");
		flock($fp, LOCK_EX);
		fseek($fp, 0, SEEK_END);
		fwrite($fp, time()."/".$typeExp."/".$objId."/".$format->id."/\n");
		flock($fp, LOCK_UN);
		fclose($fp);
	}
}
?>
