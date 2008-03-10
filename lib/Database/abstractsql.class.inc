<?php
require_once('dbdescription.class.inc');

class NLAbstractSQLDatabase {

	function NLAbstractSQLDatabase($dbDescription) {
	}

	function Connect() {
	}

	function Close() {
	}

	function ModifyingQuery($query, $table=NULL, $retField=NULL, $flushCache = true) {
	}

	function ModifyingCopy($tableName, $cols, $data) {
	}

	function Query($query, $donotusecache = false) {
	}

	function InsertObject($table, $fields, $values) {
	}

	function LimitOffsetString($limit, $offset) {
	}

	function TrueStatusString($status) {
	}

	function UpdateObjectById($table, $objectId, $fields, $values) {
	}

	function DateTimeString($string) {
	}

	function IsUniqueColsCombination($tables, $colValues, $exception) {
	}

	function ClearCache() {
		if ($this->cache == true) {
			$d = new NLDir(BASEPATH.'/tmp/cache/db');
			$d->Emptyfy();
		}
	}

	function GetTableInfo($tableName) {
		return array();
	}

	function GetError() {
		return "Error!";
	}

	function EscapeString($string) {
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

}

?>
