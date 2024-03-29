<?php
/**
 * Целочисленный плоский одномерный массив
 * В базе может храниться как integer[] при условии, что будет содержать плоский одномерный список значений
 *
 * displayType HIDDEN, STATIC, TABLE
 * storageType flatintarray
 */
class PXStorageTypeFlatIntArray extends PXStorageTypeIntArray {

    public const defaultSQLType = 'INTEGER[]';

	protected function prepareValue($value) {
		$value[0] = '[';
		$value[strlen((string) $value) - 1] = ']';
		return $value;
	}
}
