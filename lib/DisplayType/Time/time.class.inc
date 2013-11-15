<?php
/**
 * Время (дата скрыта, но как правило текущая,
 * поэтому делать сортировку по этому полю не стоит)
 *
 * storageType timestamp
 * displayType TIME
 */
class PXDisplayTypeTime extends PXDisplayTypeTimestamp {
	function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = (isset($object[$k]) && $object[$k] !== 'null') ? $object[$k] : ($field->defaultValue == 'null' ? NULL : date('j-n-Y G:i:s'));

		return NLAbstractHTMLForm::BuildDateTime($k, $value, true, false, false);
	}

	function buildCell($field, $object, $param=NULL) {
		return mb_substr($object[$field->name], 11);
	}
}
?>
