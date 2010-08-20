<?php
/**
 * Таблица URL регионов
 *
 * storageType regionurl
 * displayType URLTABLE
 */
class PXDisplayTypeUrltable extends PXDisplayTypeTable {

	function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? explode(',', trim($object[$field->name])) : array();
		foreach ($value as $k => $v) {
			$value[$k] = (array) $v;
		}

		$height = 0;
		$cols = '1';
		$header = array();

		return NLAbstractHTMLForm::BuildTableByCols($field->name, $value, $cols, $height, $header);
	}

}

?>
