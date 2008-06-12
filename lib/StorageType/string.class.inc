<?php
/**
 * Строка
 * В базе может храниться как VARCHAR(x), так и TEXT
 *
 * displayType HIDDEN, TEXT, RICHEDIT, DROPDOWN, COLOR, STATIC
 * storageType string
 */
class PXStorageTypeString extends PXStorageType {
	static function objectsSortOrder($field, $object, &$param) {
		$html = '<a ';

		switch($param['order']) {
			case ($field->name.' DESC') :
				$html .= ' class="down"';
				$order =  $field->name;
				$title = '';
				break;

			case $field->name :
				$html .= ' class="up"';
				$order = $field->name.' DESC';
				$title = ' в обратном порядке';
				break;

			default:
				$order =  $field->name;
				$title = '';
				break;
		}

		$html .= ' href="'.$param['table']->_BuildHref($param['dtype'].'_order', $order).'"';
		$html .= ' title="Сортировать по полю &bdquo;'.$field->description.'&ldquo;'.$title.'">'.$field->description.'</a>';

		return $html;
	}

	static function getFromRequest($field, $object, &$param) {
		return (string)$object[$field->name];
	}

	static function normalizeObjectAttribute($field, $object, &$param) {
		$k = $field->name;
		$v = $object[$k];

		if(is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}

		if(is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return array($v, $param['dbFields'], $param['dbValues']);
	}

	static function searchLikeQuery($field, $object, &$param) {
		return $param['formatId'].'.'.$field->name." LIKE '%".$param['word']."%' OR ";
	}
}
?>
