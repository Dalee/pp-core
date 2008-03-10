<?php
require_once('dbdescription.class.inc');

class NLSQLDatabaseAdapter {
	var $db;
	var $cache;
	var $connection;

	function NLSQLDatabaseAdapter($dbDescription) {
		$this->Init($dbDescription);
	}

	function Init($dbDescription) {
		switch ($dbDescription->dbtype) {
			case 'pgsql':
				require_once('pgsql.class.inc');
				$this->db = new NLPGSQLDatabase($dbDescription);
				break;
			case 'mssql':
				require_once('mssql.class.inc');
				$this->db = new NLMSSQLDatabase($dbDescription);
				break;
			case 'mysql':
				require_once('mysql.class.inc');
				$this->db = new NLMYSQLDatabase($dbDescription);
				break;
			case NULL:
			case '':
				FatalError("Вы не выбрали тип базы данных");
				break;
			default:
				FatalError("Указан некорректный тип базы данных");
				break;
		}
		$this->cache      = $dbDescription->cache;
		$this->connection = &$this->db->connection;
	}

	function SwitchDatabase($dbDescription) {
		$this->db->Close();
		$this->Init($dbDescription);
	}

	function Connect() {
		return $this->db->Connect();
	}

	function Close() {
		return $this->db->Close();
	}

	function ModifyingQuery($query, $table=NULL, $retField=NULL, $flushCache = true) {
		return $this->db->ModifyingQuery($query, $table, $retField, $flushCache);
	}

	function ModifyingCopy($tableName, $cols, $data) {
		return $this->db->ModifyingCopy($tableName, $cols, $data);
	}

	function Query($query, $donotusecache = false, $limitpair=NULL) {
		return $this->db->Query($query, $donotusecache, $limitpair);
	}

	function InsertObject($table, $fields, $values) {
		return $this->db->InsertObject($table, $fields, $values);
	}

	function UpdateObjectById($table, $objectId, $fields, $values) {
		return $this->db->UpdateObjectById($table, $objectId, $fields, $values);
	}

	function TrueStatusString($status='TRUE') {
		return ($status == 'TRUE' || $status == 1) ? "'1'" : "'0'";
	}

	function TransactionString($begin=TRUE) {
		return $this->db->TransactionString($begin);
	}

	function importDateTime($string) {
		return $this->db->importDateTime($string);
	}

	function importBoolean($string) {
		return $this->db->importBoolean($string);
	}

	function IsUniqueColsCombination($tables, $colValues, $exception) {
		return $this->db->IsUniqueColsCombination($tables, $colValues, $exception);
	}

	function ClearCache() {
		return $this->db->ClearCache();
	}

	function GetTableInfo($tableName) {
		return $this->db->GetTableInfo($tableName);
	}

	function GetError() {
		return $this->db->GetError();
	}

	function EscapeString($string) {
		return $this->db->EscapeString($string);
	}

	function mapFields($fieldName) {
		return $this->db->mapFields($fieldName);
	}

	function MapData($value) {
		return $this->db->MapData($value);
	}

	function exportFloat($value) {
		return $this->db->exportFloat($value);
	}

	function exportDateTime($value) {
		return $this->db->exportDateTime($value);
	}

	function tableExists($tableName) {
		return $this->db->tableExists($tableName);
	}
}

?>
