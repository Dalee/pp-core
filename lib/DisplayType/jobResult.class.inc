<?php

/**
 * Class PXDisplayTypeJobResult.
 */
class PXDisplayTypeJobResult extends PXDisplayType {

	public function buildInput($field, $object, $param = null) {
		$html = '';
		$value = isset($object[$field->name]) ? $object[$field->name] : [];

		$keys = explode('-', $field->displayTypeArgs[0]);
		$titles = explode('-', $field->displayTypeArgs[1]);

		foreach ($keys as $idx => $key) {
			$title = $titles[$idx];
			$values = getFromArray($value, $key, []);

			if (!empty($values)) {
				$html .= sprintf('<h2>%s</h2>', $title);
				$html .= join('<br />', $values);
			}
		}

		return $html;
	}

	public function buildCell($field, $object, $param = null) {
		return '';
	}

}
