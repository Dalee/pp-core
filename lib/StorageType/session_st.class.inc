<?php
/**
 * Класс хранения в сессии
 *
 * displayType HIDDEN
 */
class PXStorageTypeSession extends PXStorageType {
	function normalize($field, $object, &$param, $allowed = NULL) {
		$s = new PXSession('session_'.$field->name);
		$s->open();
		
		$normalized =& $s->getSessionVars();

		$s->close();
	
		return $normalized;
	}
	
	function proceedFile($field, &$object, &$param, $allowed = null) {
		$s = new PXSession('session_'.$field->name);
		$s->open();

		$vars =& $s->getSessionVars();
		if (is_array($vars) && count($vars) && is_array($object[$field->name])) {
			$vars = array_merge($vars, $object[$field->name]);
		} else {
			$vars = $object[$field->name];
		}

		$s->close();
		return '';
	}

	function notInDb($field, $object, $param=NULL) {
		return true;
	}
}
?>
