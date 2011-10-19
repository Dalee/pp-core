<?php
class PXStorageTypeFlatIntArray extends PXStorageTypeArray {
	function valueToDB( $k, $v ) {
		return ( $v === '' || is_null( $v ) ) ? 'null' : (int)$v;
	}

	function normalize( $field, $object, &$param ) {
		$value = $object[$field->name];

		$value = str_replace( '{', '[', $value );
		$value = str_replace( '}', ']', $value );
		return json_decode( $value );
	}
}

?>
