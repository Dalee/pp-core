<?php

class PXTriggerDatabaseDeferredStatus extends PXAbstractDatabaseTrigger {

	protected $fields2apply = array(
		'status',
		'sys_deferred_state',
		'sys_deferred_since',
		'sys_deferred_until',
	);

	function OnBeforeAddObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues) {
		if (!isset($format->fields['sys_deferred_state'])) return true;
		if (!$object['sys_deferred_state']) return true;
		return $this->apply($object, $dbFields, $dbValues);
	}

	function OnBeforeModifyObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB) {
		if (!isset($format->fields['sys_deferred_state'])) return true;
		if (!$object['sys_deferred_state']) return true;
		return $this->apply($object, $dbFields, $dbValues);
	}

	function apply($object, $dbFields, &$dbValues) {
		PXDeferredStatusHelper::applyNextState($object);
		foreach ($this->fields2apply as $k) {
			$n = array_search($k, $dbFields);
			$dbValues[$n] = $object[$k];
		}
		return true;
	}

}
