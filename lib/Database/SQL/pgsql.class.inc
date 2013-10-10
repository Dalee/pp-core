<?php

require_once PPLIBPATH . 'Database/SQL/abstract.class.inc';

class NLPGSQLDatabase extends NLAbstractSQLDatabase {
	var $connectString;
	var $connection;
	var $cache;
	var $connected;

	//Allowed: array('host', 'hostaddr', 'port', 'dbname', 'user', 'password', 'connect_timeout', 'options', 'tty', 'sslmode', 'requiressl', 'service')
	protected $includeOptions = array('connect_timeout' => '');
	protected $excludeOptions = array('encoding');

	function __construct($dbDescription) {
		$this->setCache($dbDescription->cache);
		$this->connectArray = array_merge($this->connectArray, $this->includeOptions);
		$this->setConnectParams($dbDescription);
		$this->connected = false;
	}

	function setConnectParams($dbDescription) {
		$keys = array_keys($this->connectArray);
		$connectString = array();
		foreach($keys as $key) {
			if ($dbDescription->$key !== NULL) {
				$this->connectArray[$key] = $dbDescription->$key;
				if (!in_array($key, $this->excludeOptions)) {
					$connectString[] = $key . '=' . $dbDescription->$key;
				}
			}
		}
		$this->connectString = trim(implode(' ', $connectString));
	}

	function Connect() {
		if (!$this->connected) {
			$this->connection = pg_connect($this->connectString, PGSQL_CONNECT_FORCE_NEW) or FatalError('Can\'t connect to '.$this->connectString);

			if ($this->connection) {
				pg_exec($this->connection, "SET DATESTYLE='German'");

				if (!empty($this->connectArray['encoding'])) {
					pg_set_client_encoding($this->connection, $this->connectArray['encoding']);
				}

				$this->connected  = true;
			}
		}
		return $this->connected;
	}

	function Close() {
		if ($this->connected) {
			pg_close($this->connection);
			$this->connected = false;
		}
	}

	function ModifyingQuery($query, $table=NULL, $retField=NULL, $flushCache = true, $retCount = false) {
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
			} elseif ($retCount) {
				return pg_affected_rows($res);
			} else {
				PXProfiler::end();
				return $res;
			}

		} else {
			return ERROR_DB_CANNOTCONNECT;
		}
	}

	function ModifyingCopy($tableName, $cols, $data) {
		PXProfiler::begin('SQL: COPY '.sizeof($cols).'x'.sizeof($data).' to '.$tableName);

		if (!$this->connected) {
			$this->Connect();
		}

		$row = reset($data);
		if (empty($row)) {
			PXProfiler::end();
			return ERROR_DB_BADQUERY;
		}

		if ($this->connected) {

			$query = "COPY {$tableName} (\"".implode('", "', $cols)."\") FROM stdin";

			if (pg_query($this->connection, $query) == false) {
				PXProfiler::end();
				return ERROR_DB_BADQUERY;
			}

			$from = array( "\\", "\r", "\n", "\t", "\1" );
			$to = array( "\\\\", "\\r", "\\n", "\\t", "\t");
			$lines = array();

			do {
				$line = str_replace($from, $to, implode("\1", $row));

				while (false !== strpos($line, "\t\t")) {
					$line = str_replace("\t\t", "\t\\N\t", $line);
				}

				if ($line[0] == "\t") {
					$line = "\\N" . $line;
				}

				if ($line[strlen($line)-1] == "\t") {
					$line .= "\\N";
				}

				$lines[] = $line;

			} while($row = next($data));

			if (pg_put_line($this->connection, join("\n", $lines)."\n") == false) {
				PXProfiler::end();
				return ERROR_DB_BADQUERY;
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

	function InsertObject($table, $fields, $values, $flushCache = true) {
		$query = "INSERT INTO {$table} (\"".implode('", "', $fields)."\") VALUES (".implode(', ', array_map(array($this, '__mapInsertData'), $values)).")";
		$id    = $this->modifyingQuery($query, $table, 'id', $flushCache);
		return $id;
	}

	function MapData($value) {
		switch(true) {
			case is_null($value) || $value === '' :
				return "NULL";
			case $value === "##now##" || $value === "now()":
				return "now()";
			case is_bool($value):
				return $value ? "'t'" : "'f'";
			default:
				return "'".$this->EscapeString($value)."'";
		}
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

	function UpdateObjectById($table, $objectId, $fields, $values, $flushCache = true) {
		$query = "UPDATE {$table} SET ".implode(', ', array_map(array($this, '__mapUpdateData'), $fields, $values))." WHERE id={$objectId}";
		return $this->ModifyingQuery($query, null, null, $flushCache);
		return NULL;
	}

	/*
	 * Transaction related functions
	 *
	 */
	protected $insideTransaction = false;
	protected $transactionsStack = array();

	function transactionBegin() {
		if (!$this->insideTransaction) {
			$this->Query('BEGIN',true);
			$this->insideTransaction = true;
		} else { // emulating nested transactions
			$savepointId = uniqid('px_');
			$this->savepointCreate($savepointId);
			$this->transactionsStack[] = $savepointId;
		}
	}

	function transactionCommit() {
		if (empty($this->transactionsStack)) {
			$this->Query('END',true);
			$this->insideTransaction = false;
		} else { // emulating nested transactions
			$savepointId = array_pop($this->transactionsStack);
			$this->savepointRelease($savepointId);
		}
	}

	function transactionRollback() {
		if (empty($this->transactionsStack)) {
			$this->Query('ROLLBACK',true);
			$this->insideTransaction = false;
		} else { // emulating nested transactions
			$savepointId = array_pop($this->transactionsStack);
			$this->savepointRollbackTo($savepointId);
		}
	}

	/**
	 * Named transactions (savepoints) related functions
	 */
	function savepointCreate($id) {
		$this->Query('SAVEPOINT '.$id, true);
	}

	function savepointRelease($id) {
		$this->Query('RELEASE SAVEPOINT '.$id, true);
	}

	function savepointRollbackTo($id) {
		$this->Query('ROLLBACK TO '.$id, true);
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
		return $string == 't' || $string == '1';
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

	function getError() {
		return pg_last_error();
	}

	function tableExists($tableName) {
		return count($this->query("SELECT relname FROM pg_class WHERE relname='{$tableName}'"));
	}
	
	function LIKE($condition, $percs){
		return $this->_searchMethod("ILIKE", $condition, $percs);
	}

	function inArray($arrayField, $value, $sane = false){
		return sprintf("%s @> ARRAY[%s]", $sane ? $arrayField : $this->EscapeString($arrayField), $sane ? $value : $this->EscapeString($value));
	}
	
	function arrayIn($arrayField, $value, $sane = false){
		return sprintf("%s <@ ARRAY[%s]", $sane ? $arrayField : $this->EscapeString($arrayField), $sane ? $value : $this->EscapeString($value));
	}

	function intersectIntArray($arrayField, $values) {
		return sprintf("%s && '{%s}'",
			$this->escapeString($arrayField),
			join(",", array_filter($values, "is_numeric")));
	}

	function ifStatement($when, $then, $else = null) {
		$else = $else? "ELSE ({$else})" : '';
		return sprintf("CASE WHEN (%s) THEN (%s) %s END", $when, $then, $else);
	}

	function caseStatement($arr, $else = null) {
		$else = $else? "ELSE ({$else})" : '';
		$whenthens = '';
		foreach ((array)$arr as $when => $then) {
			$whenthens .= sprintf('WHEN (%s) THEN (%s) ', $when, $then);
		}
		return sprintf("CASE %s %s END", $whenthens, $else);
	}

	function vacuumTable($tableName) {
		$this->query("VACUUM ".$tableName, true);
		$this->query("VACUUM ANALYZE ".$tableName, true);
	}

	function loggerSqlFormat($table, $fields) {
		if (!count($fields)) return false;
		$fieldNames = implode(', ', array_map(array(&$this, "mapFields"), array_keys($fields)));
		$fieldValues = implode(', ', array_map(array(&$this, "MapData"), array_values($fields)));
		return sprintf("INSERT INTO %s (%s) VALUES(%s)", $table, $fieldNames, $fieldValues);
	}
}
?>
