<?php
/**
 * Сериализованный массив
 * В базе может храниться как integet[], string[]
 *
 * displayType HIDDEN, STATIC, TABLE
 * storageType serialized
 */

class PXStorageTypeArray extends PXStorageType {
	function getFromRequest($field, $object, &$param) {
		$k = $field->name;
		return (is_array($object[$k])) ? $object[$k] : array();
	}
 
	function normalizeObjectAttribute($field, $object, &$param) {
		$value = $object[$field->name];
		if (!count($value)) {
			$value = array();
		}

		foreach ($value as $k => $v) {
			$v = implode('","', $v);

			if(!strlen($v)) {
				unset($value[$k]);

			} else {
				$value[$k] = '{"'.$v.'"}';
			}
		}

		$valueInDb = '{'.implode(',', $value).'}';

		if(is_array($param['dbFields'])) {
			$param['dbFields'][] = $field->name;
		}
		 
		if(is_array($param['dbValues'])) {
			$param['dbValues'][] = $valueInDb;
		}
		 
		return array($valueInDb, $param['dbFields'], $param['dbValues']);
	}

	function normalize($field, $object, &$param) {
		 $value = $object[$field->name];
		 $value = split("},{", $value);
		 foreach($value as $k => $v) {
			$v = str_replace(array('{','"','}'), '',  $v);
			$value[$k] = split(",", $v);
		 }
		return (is_array($value) ? $value : array());
	 }

}

?>
