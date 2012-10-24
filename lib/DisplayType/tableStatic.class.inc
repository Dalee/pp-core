<?php
/**
 * static Таблица
 *
 * storageType serialized
 * displayType TABLESTATIC
 */
class PXDisplayTypeTableStatic extends PXDisplayTypeStatic {
	function buildInput($field, $object, $param = null) {
		$k = $field->name;
		$value = isset($object[$k]) && is_array($object[$k]) ? $object[$k] : array();
		$safe = in_array('safe', $field->displayTypeArgs);

		$_html   = '<table class="static">';

		foreach($value as $row) {
			$_html .= '<tr>';

			foreach($row as $cell) {
				is_array($cell) && $cell = print_r($cell, true);
				$safe && $cell = htmlentities($cell, ENT_COMPAT, 'koi8-r');
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
?>