<?php
/**
 * Таблица
 *
 * storageType serialized
 * displayType TABLE|col1width-..-colNwidth|height
 *             col1width-..-colNwidth относительная ширина столбцов, например 1-2-1-1
 *             если height = 0, то в каждой ячейке таблицы будет использоваться <input type="text">
 *             если height > 0, то в каждой ячейке таблицы будет использоваться <textarea> и height это ее высота в пикселях
 */
class PXDisplayTypeTable extends PXDisplayType {
	function buildInput($field, $object, $param=NULL) {
		$value  = isset($object[$field->name]) ? htmlspecialcharsArray($object[$field->name]) : array();
		$height = isset($field->displayTypeArgs[1]) ? $field->displayTypeArgs[1] : 0;

		return NLAbstractHTMLForm::BuildTableByCols($field->name, $value, explode("-", $field->displayTypeArgs[0]), $height);
	}

	function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetArrayVar($field->name);
	}
}
?>