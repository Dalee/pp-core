<?php
/**
 * Дата (время скрыто, но как правило текущее)
 *
 * storageType timestamp
 * displayType DATE
 */
class PXDisplayTypeDate extends PXDisplayTypeTimestamp {
	function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = (isset($object[$k]) && $object[$k] !== 'null') ? $object[$k] : ($field->defaultValue == 'null' ? NULL : date('j-n-Y G:i:s'));

		return NLAbstractHTMLForm::BuildDateTime($k, $value, false, true, true);
	}

	function buildCell($field, $object, $param=NULL) {
		return mb_substr($object[$field->name], 0, 10);
	}
}
?>
