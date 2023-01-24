<?php
/**
 * static Таблица
 *
 * storageType serialized
 * displayType TABLESTATIC
 */
class PXDisplayTypeTableStatic extends PXDisplayTypeStatic
{
	public function buildInput($field, $object, $param = null)
	{
		$k = $field->name;
		$value = isset($object[$k]) && is_array($object[$k]) ? $object[$k] : [];
		$safe = in_array('safe', $field->displayTypeArgs);

		$_html = '<table class="static">';

		foreach ($value as $row) {
			$_html .= '<tr>';

			foreach ($row as $cell) {
				is_array($cell) && $cell = print_r($cell, true);
				$safe && $cell = htmlspecialchars((string) $cell, ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET);
				$_html .= '<td>';
				$_html .= $cell;
				$_html .= '</td>';
			}

			$_html .= '</tr>';
		}

		$_html .= '</table>';

		return $_html;
	}
}
