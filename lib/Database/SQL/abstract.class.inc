<?php
require_once 'Config/Description/db.class.inc';

abstract class NLAbstractSQLDatabase {
	function __construct($dbDescription) {
	}

	function connect() {
	}

	function close() {
	}

	function setCache($isCacheOn) {
		$cacheClass = $isCacheOn ? 'ObjectCache' : 'NullCache';
		$this->cache = new $cacheClass('database');
	}

	function modifyingQuery($query, $table=NULL, $retField=NULL, $flushCache = true) {
	}

	function modifyingCopy($tableName, $cols, $data) {
	}

	function query($query, $donotusecache = false) {
	}

	function insertObject($table, $fields, $values) {
	}

	function limitOffsetString($limit, $offset) {
	}

	function trueStatusString($status='TRUE') {
		return ($status == 'TRUE' || $status == 1) ? 'TRUE' : 'FALSE';
	}

	function updateObjectById($table, $objectId, $fields, $values) {
	}

	function dateTimeString($string) {
	}

	function isUniqueColsCombination($tables, $colValues, $exception) {
	}

	function getTableInfo($tableName) {
		return array();
	}

	function getError() {
		return "Error!";
	}

	function escapeString($string) {
		return addslashes($string);
	}

	function mapFields($field) {
		return $field;
	}

	function exportFloat($value) {
		return $value;
	}

	function exportDateTime($value) {
		return $value;
	}

	function tableExists() {
		return true;
	}

	function transactionBegin() {
	}

	function transactionCommit() {
	}

	function transactionRollback() {
	}
}
?>
