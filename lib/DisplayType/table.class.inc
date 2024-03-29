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
class PXDisplayTypeTable extends PXDisplayType
{
	public function buildInput($field, $object, $param = NULL)
	{
		$value = $object[$field->name] ?? [];
		$cols = $field->displayTypeArgs[0] ?? '1';
		$height = $field->displayTypeArgs[1] ?? 0;
		$header = isset($field->displayTypeArgs[2]) ? explode($field->displayTypeArgs[3] ?? '-', (string) $field->displayTypeArgs[2]) : [];

		return NLAbstractHTMLForm::BuildTableByCols($field->name, $value, explode("-", (string) $cols), $height, $header);
	}

	public function buildCell($field, $object, $param = NULL)
	{
		$value = isset($object[$field->name]) ? htmlspecialcharsArray($object[$field->name]) : [];

		$html = '';
		$html_len = 0;

		for ($i = 0, $l = is_countable($value) ? count($value) : 0; $i < $l && $html_len < 30; $i++) {
			$html .= (!empty($html)) ? '; ' : '';
			$html .= join(', ', (array)$value[$i]);

			$html_len = mb_strlen($html);
		}
		($i < $l) && $html .= '; &hellip;';

		return $html;
	}

	public function getFromRequest($field, $object, $param = NULL)
	{
		return $param['request']->postData->_GetArrayVar($field->name);
	}
}
