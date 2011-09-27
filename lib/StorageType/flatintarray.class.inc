<?php
class PXStorageTypeFlatIntArray extends PXStorageTypeArray {
	function valueToDB( $k, $v ) {
		return ( $v === '' || is_null( $v ) ) ? 'null' : (int)$v;
	}

	function normalize( $field, $object, &$param ) {
		$value = $object[$field->name];

		$str = substr( $value, 1, strlen( $value ) - 2 );

		return explode( ',', $str );
	}
}

?>
