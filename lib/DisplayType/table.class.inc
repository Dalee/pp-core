<?php
/**
 * Таблица
 *
 * storageType serialized
 * displayType TABLE|col1width-..-colNwidth|height|col1header-..-colNheader|headersDelimiter
 *             col1width-..-colNwidth относительная ширина столбцов, например 1-2-1-1
 *             если height = 0, то в каждой ячейке таблицы будет использоваться <input type="text">
 *             если height > 0, то в каждой ячейке таблицы будет использоваться <textarea> и height это ее высота в пикселях
 *             col1header-..-colNheader заголовки колонок таблицы, например Номер-Наименование-Цена-Количество
 *             headersDelimiter опциональный разделитель заголовков колонок таблицы, отличный от "-", например, если в одном из заголовков присутствует символ "-"
 */
class PXDisplayTypeTable extends PXDisplayType {
	function buildInput($field, $object, $param=NULL) {
		$value  = isset($object[$field->name]) ? htmlspecialcharsArray($object[$field->name]) : array();
		$cols   = isset($field->displayTypeArgs[0]) ? $field->displayTypeArgs[0] : '1';
		$height = isset($field->displayTypeArgs[1]) ? $field->displayTypeArgs[1] : 0;
		$header = isset($field->displayTypeArgs[2]) ? explode(isset($field->displayTypeArgs[3]) ? $field->displayTypeArgs[3] : '-', $field->displayTypeArgs[2]) : array();

		return NLAbstractHTMLForm::BuildTableByCols($field->name, $value, explode("-", $cols), $height, $header);
	}

	function buildCell($field, $object, $param=NULL) {
		$value  = isset($object[$field->name]) ? htmlspecialcharsArray($object[$field->name]) : array();

		$html = '';
		for ($i = 0, $l = count($value); $i < $l && strlen($html) < 30; $i++) {
			!empty($html) && $html .= '; ';
			$html .= join(', ', (array)$value[$i]);
		}
		($i < $l) && $html .= '; ...';

		return $html;
	}

	function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetArrayVar($field->name);
	}
}
?>