<?php
/**
 * "Виртуальное" поле
 * нигде не хранится, значение берется или из значения по умолчанию ($field->defaultValue)
 * или позже вычисляется в PXOutputHandler'ах
 *
 * displayType STATIC
 * storageType virtual
 */
class PXStorageTypeVirtual extends PXStorageType {
	function normalize($field, $object, &$param) {
		return (!is_null($field->defaultValue)) ? $field->defaultValue : NULL;
	}

	function notInDb($field, $object, $param=NULL) {
		return true;
	}
}
?>