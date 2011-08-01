<?php
/**
 * static Таблица
 *
 * storageType serialized
 * displayType TABLESTATIC
 */
class PXDisplayTypeTableStatic extends PXDisplayTypeStatic {
	function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = isset($object[$k]) && is_array($object[$k]) ? $object[$k] : array();

		$_html   = '<table class="static">';

		foreach($value as $row) {
			$_html .= '<tr>';

			foreach($row as $cell) {
				$_html .= '<td>';
				$_html .= is_array($cell) ? print_r($cell, true) : $cell;
				$_html .= '</td>';
			}

			$_html .= '</tr>';
		}

		$_html .= '</table>';

		return $_html;
	}
}
?>