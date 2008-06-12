<?php
/**
 * Дата (время скрыто, но как правило текущее)
 *
 * storageType timestamp
 * displayType DATE
 */
class PXDisplayTypeDate extends PXDisplayTypeTimestamp {
	static function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = (isset($object[$k]) && $object[$k] !== 'null') ? $object[$k] : ($field->defaultValue == 'null' ? NULL : date('j-n-Y G:i:s'));

		return NLAbstractHTMLForm::BuildDateTime($k, $value, false, true, true);
	}

	static function buildCell($field, $object, $param=NULL) {
		return substr($object[$field->name], 0, 10);
	}
}
?>
