<?php
require_once 'abstract.class.inc';

class NLPGSQLDatabase extends NLAbstractSQLDatabase {

	var $connectString;
	var $connection;
	var $cache;
	var $encoding;
	var $connected;

	function NLPGSQLDatabase($dbDescription) {
		$this->setCache($dbDescription->cache);

		$this->connectString   = $dbDescription->GetPgsqlConnectString();
		$this->encoding        = $dbDescription->encoding;
		$this->connected       = false;
	}

	function Connect() {
		if (!$this->connected) {
			$this->connection = @pg_connect($this->connectString, PGSQL_CONNECT_FORCE_NEW) or FatalError('Can\'t connect to '.$this->connectString);

			if ($this->connection) {
				pg_exec($this->connection, "SET DATESTYLE='German'");

				if ($this->encoding) {
					pg_set_client_encoding($this->connection, $this->encoding);
				}

				$this->connected  = true;
			}
		}
	}

	function Close() {
		if ($this->connected) {
			pg_close($this->connection);
			$this->connected = false;
		}
	}

	function ModifyingQuery($query, $table=NULL, $retField=NULL, $flushCache = true) {
		if (!$this->connected) {
			$this->Connect();
		}

		if ($this->connected) {
			PXProfiler::begin('SQL: '.$query);

			if (($res = pg_query($this->connection, $query)) == false) {
				PXProfiler::end();
				return ERROR_DB_BADQUERY;
			}

			if ($flushCache == true) {
				$this->cache->clear();
			}

			if ($table && $retField) {
				$table = $this->Query("SELECT {$retField} FROM {$table} WHERE oid = ".pg_last_oid($res), true);

				if (is_array($table) && count($table)) {
					PXProfiler::end();
					return $table[0][$retField];
				} else {
					PXProfiler::end();
					return NULL;
				}

			} else {
				PXProfiler::end();
				return $res;
			}

		} else {
			return ERROR_DB_CANNOTCONNECT;
		}
	}

	function ModifyingCopy($tableName, $cols, &$data) {
		PXProfiler::begin('SQL: COPY '.sizeof($cols).'x'.sizeof($data).' to '.$tableName);

		if (!$this->connected) {
			$this->Connect();
		}

		if ($this->connected) {
			$query = "COPY {$tableName} (\"".implode('", "', $cols)."\") FROM stdin";

			if (pg_query($this->connection, $query) == false) {
				PXProfiler::end();
				return ERROR_DB_BADQUERY;
			}

			foreach ($data as $k=>$row) {
				$line = implode(chr(1), $row);
				$line = str_replace("\\", "\\\\", $line);
				$line = str_replace("\r", "\\r", $line);
				$line = str_replace("\n", "\\n", $line);
				$line = str_replace("\t", "\\t", $line);
				$line = str_replace(chr(1), "\t", $line);

				while (strstr($line, "\t\t")) {
					$line = str_replace("\t\t", "\t\\N\t", $line);
				}

				if ($line{0} == "\t") {
					$line = "\\N\t" . substr($line, 1);
				}
				
				if ($line{strlen($line)-1} == "\t") {
					$line .= "\\N";
				}

				if (pg_put_line($this->connection, $line."\n") == false) {
					PXProfiler::end();
					return ERROR_DB_BADQUERY;
				}
			}

			if (pg_put_line($this->connection, "\\.\n") == false) {
				PXProfiler::end();
				return ERROR_DB_BADQUERY;
			}

			if (pg_end_copy($this->connection) == false) {
				PXProfiler::end();
				return ERROR_DB_BADQUERY;
			}

		} else {
			PXProfiler::end();
			return ERROR_DB_CANNOTCONNECT;
		}

		PXProfiler::end();
		return true;
	}

	function Query($query, $donotusecache = false, $limitpair = NULL) {
		if (is_array($limitpair)) {
			$limitstring = " LIMIT {$limitpair[0]} OFFSET {$limitpair[1]}";
		} else {
			$limitstring = "";
		}

		$query .= $limitstring;

		PXProfiler::begin('SQL: '.$query);

		if (!is_null($table = $this->_loadFromCache($query, $donotusecache))) { 
			return $table; 
		}

		if (!$this->connected) {
			$this->Connect();
		}

		if ($this->connected) {
			if (($res = pg_query($this->connection, $query)) === false) {
				PXProfiler::end();
				return ERROR_DB_BADQUERY;
			}

			$table = array();
			$total = pg_num_rows($res);

			for ($i=0; $i<$total; $i++) {
				$table[] = pg_fetch_assoc($res, $i);
			}

			$this->_saveToCache($query, $table, $donotusecache);
		} else {
			PXProfiler::end();
			return ERROR_DB_CANNOTCONNECT;
		}

		PXProfiler::end();
		return $table;
	}


	function InsertObject($table, $fields, $values) {
		$query = "INSERT INTO {$table} (\"".implode('", "', $fields)."\") VALUES (".implode(', ', array_map(array(&$this, '__mapInsertData'), $values)).")";
		$id    = $this->modifyingQuery($query, $table, 'id');
		return $id;
	}

	function MapData($value) {
		if (is_null($value) || $value === '') return "NULL";
		else if ($value === "##now##")        return "now()";
		else if ($value === "now()")          return "now()";
		else return "'".$this->EscapeString($value)."'";
	}
	
	function mapFields($field) {
		return '"'.$this->EscapeString($field).'"';
	}

	function __mapInsertData($value) {
		return $this->MapData($value);
	}

	function __mapUpdateData($field, $value) {
		return "\"{$field}\" = ".$this->MapData($value);
	}

	function EscapeString($s) {
		return pg_escape_string($s);
	}

	function UpdateObjectById($table, $objectId, $fields, $values) {
		$query = "UPDATE {$table} SET ".implode(', ', array_map(array(&$this, '__mapUpdateData'), $fields, $values))." WHERE id={$objectId}";
		return $this->ModifyingQuery($query);
		return NULL;
	}

	/*
	 * Transaction related functions
	 * 
	 */
	function transactionBegin() {
		$this->Query('BEGIN',true);
	}

	function transactionCommit() {
		$this->Query('END',true);
	}

	function transactionRollback() {
		$this->Query('ROLLBACK',true);
	}

	function importDateTime($string) {
		return $string;
	}

	function exportFloat($string) {
		return str_replace(',', '.', $string);
	}

	function exportDateTime($string) {
		return $string == '00.00. 00:00:00' ? NULL : $string;
	}

	function importBoolean($string) {
		return ($string == 't' || $string == '1') ? true : false;
	}

	function IsUniqueColsCombination($tables, $colValues, $exception) {
		if (!is_array($tables) || !sizeof($tables)) {
			FatalError("Вы не указали проверяемые таблицы");
		}

		if (!is_array($colValues) || !sizeof($colValues)) {
			FatalError("Вы не указали проверяемые столбцы");
		}

		$query = "SELECT (0 ";
		foreach($tables as $t) {
			list($where_clauses, $tableName) = array(array(), $t['tableName']);

			foreach($colValues as $c => $v) {
				$where_clauses[] = sprintf("%s = '%s'", $c, $v);
			}

			if(sizeof($exception)) {
				foreach($exception as $c=>$v) {
					$where_clauses[] = sprintf("%s != '%s'", $c, $v);
				}
			}

			$query = sprintf("%s + (SELECT count(*) FROM %s WHERE %s", 
				$query, $tableName, join(" AND ", $where_clauses));

			if($t['exWhere']) {
				$query = sprintf("%s and %s", $query,  $t['exWhere']);
			}

			$query .= ")";
		}
		$query .= ")";

		return (int)(current(current($this->Query($query, TRUE))));
	}

	function GetTableInfo($tableName) {
		if (!$this->connected) {
			$this->Connect();
		}

		$res = pg_meta_data($this->connection, $tableName);

		if (!is_array($res)) {
			return ERROR_DB_BADQUERY;
		} else {
			return $res;
		}
	}

	function getError() {
		return pg_last_error();
	}

	function tableExists($tableName) {
		return count($this->query("SELECT relname FROM pg_class WHERE relname='{$tableName}'"));
	}
	
	function LIKE($condition, $percs){
		return $this->_searchMethod("ILIKE", $condition, $percs);
	}

	function inArray($arrayField, $value){
		return sprintf("%s @> ARRAY[%s]", $this->EscapeString($arrayField), $this->EscapeString($value));
	}
}
?>
