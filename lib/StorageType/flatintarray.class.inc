<?php
class PXStorageTypeFlatIntArray extends PXStorageTypeArray {
	function valueToDB( $k, $v ) {
		return ( $v === '' || is_null( $v ) ) ? 'null' : (int)$v;
	}

	function normalize( $field, $object, &$param ) {
		$value = $object[$field->name];

		$value = strtolower($value); // special case: NULL -> null

		$value{0} = '[';
		$value{strlen($value) - 1} = ']';

		return ($array = json_decode($value)) === null ? parent::normalize($field, $object, $param) : $array;
	}
}

?>
