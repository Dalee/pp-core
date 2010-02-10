<?php
/**
 * Большое целое число
 * В базе может храниться как BIGINT
 *
 * displayType HIDDEN, TEXT, DROPDOWN, STATIC, PARENTDROPDOWN, SELFPARENTDROPDOWN, IPADDR
 * storageType bigint
 */
class PXStorageTypeBigint extends PXStorageTypeInteger {
	function getFromRequest($field, $object, &$param) {
		$k = $field->name;
		return !empty($object[$k]) ? sprintf("%u", $object[$k]) : NULL;
	}
}

?>
