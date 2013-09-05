<?php

require_once PPLIBPATH . 'Database/SQL/abstract.class.inc';

class NLMYSQLDatabase extends NLAbstractSQLDatabase {
	var $connection;
	var $cache;
	var $connected = false;

	function NLMYSQLDatabase($dbDescription) {
		$this->setCache($dbDescription->cache);
		$this->setConnectParams($dbDescription);
	}
	
	function setConnectParams($dbDesc) {
		$this->connectArray['port'] = '3306';
		foreach(array_keys($this->connectArray) as $field) {
			if ($dbDesc->$field !== NULL) {
				$this->connectArray[$field] = $dbDesc->$field;
			}
		}
	}

	function Connect() {
		if (!$this->connected) {
			$this->connection = @mysql_connect($this->connectArray['host'].':'.$this->connectArray['port'], $this->connectArray['user'], $this->connectArray['password'], true);
			if ($this->connection) {
				if (@mysql_select_db($this->connectArray['dbname'], $this->connection)) {
					$this->connected  = true;
					if (!empty($this->connectArray['encoding'])) {
						$this->Query("SET NAMES '{$this->connectArray['encoding']}'");
					}
				}
			}
		}
		return $this->connected;
	}

	function Close() {
		if ($this->connected) {
			mysql_close($this->connection);
			$this->connected = false;
		}
	}

	function ModifyingQuery($query, $table=NULL, $retField=NULL, $flushCache = true) {
		if (!$this->connected) {
			$this->Connect();
		}

		if ($this->connected) {
			if (($res = mysql_query($query, $this->connection)) == false) {
				return ERROR_DB_BADQUERY;
			}

			if ($flushCache == true) {
				$this->cache->clear();
			}

			if ($table && $retField) {
				return mysql_insert_id($this->connection);
			} else {
				return $res;
			}

		} else {
			return ERROR_DB_CANNOTCONNECT;
		}
	}

	function MapData($value) {
		if (is_null($value) || $value === '') return "NULL";
		else if ($value === "##now##")        return "now()";
		else if ($value === "now()")          return "now()";
		else return "'".$this->EscapeString($value)."'";
	}
	
	function escapeField($field) {
		return (is_array($field) ? array_map(array(&$this, 'escapeField'), $field) : '`' . $this->EscapeString($field) . '`');
	}
	
	function mapFields($string) {
		return $this->escapeField($string);
	}

	function __mapInsertData($value) {
		return $this->MapData($value);
	}

	function __mapUpdateData($field, $value) {
		return "`{$field}` = ".$this->MapData($value);
	}

	function EscapeString($s) {
		return mysql_escape_string($s);
	}	

	function ModifyingCopy($tableName, $cols, $data) {
		if (!$this->connected) {
			$this->Connect();
		}

		if ($this->connected) {
			foreach ($data as $k=>$row) {
				$this->InsertObject($tableName, $cols, $row);
			}
		} else {
			return ERROR_DB_CANNOTCONNECT;
		}
	}

	function Query($query, $donotusecache = false) {
		PXProfiler::begin('SQL: '.$query);

		if (!is_null($table = $this->_loadFromCache($query, $donotusecache))) {
			return $table;
		}

		if (!$this->connected) {
			$this->Connect();
		}

		$table = array();
		if ($this->connected) {
			if (($res = @mysql_query($query, $this->connection)) == false) {
				PXProfiler::end();
				return ERROR_DB_BADQUERY;
			}

			if (is_resource($res)) {
				while (($row = mysql_fetch_array($res, MYSQL_ASSOC))) {
					$table[] = $row;
				}
				mysql_free_result($res);

				$this->_saveToCache($query, $table, $donotusecache);
			}
		} else {
			PXProfiler::end();
			return ERROR_DB_CANNOTCONNECT;
		}

		PXProfiler::end();
		return $table;
	}

	function prepareValues($values) {
		foreach($values as &$value) {
			$norm = strtolower($value);
			switch($norm) {
				case '':
					$value = "''";
					break;
				case '##now##':
				case 'now()':
					$value = 'NOW()';
					break;
				default:
					$value = "'" . $this->EscapeString($value) . "'";
					break;
			}
		}
		return $values;
	}

	function InsertObject($table, $fields, $values) {
		$fields = $this->escapeField($fields);
		$values = $this->prepareValues($values);
		$sql = sprintf(
			"INSERT INTO %s (%s) VALUES(%s)",
			$this->escapeField($table),
			implode(',', array_values($fields)),
			implode(',', array_values($values))
		);
		
		$id = $this->ModifyingQuery($sql, $table, 'id');
		return $id;
	}

	function LimitOffsetString($limit, $offset) {
		return "LIMIT {$offset}, {$limit}";
	}

	function TrueStatusString($status) {
		return ($status == 'TRUE' || $status == 1) ? '1' : '0';
	}

	function UpdateObjectById($table, $objectId, $fields, $values) {
		$values = $this->prepareValues($values);
		if (($index = array_search('id', $fields)) !== FALSE) {
			unset($fields[$index]);
			$fields = array_values($fields);
		}
		$fields = $this->escapeField($fields);
		$values = array_combine($fields, $values);
			
		if ($values) {
			$update = array();
			foreach($values as $key => $value) {
				$update[] = $key.'='.$value;
			}
			$query = sprintf("UPDATE %s SET %s WHERE id = %d", $this->escapeField($table), implode(', ', $update), $objectId);
			$this->ModifyingQuery($query);
		}
	}

	/*
	 * Transaction related functions
	 * 
	 */
	function transactionBegin() {
		//TODO Check for storage engine is really transaction-safe?
		$this->Query('START TRANSACTION', 1);
	}

	function transactionCommit() {
		//TODO Check for storage engine is really transaction-safe?
		$this->Query('COMMIT', 1);
	}

	function transactionRollback() {
		//TODO Check for storage engine is really transaction-safe?
		$this->Query('ROLLBACK', 1);
	}

	/**
	 * Named transactions (savepoints) functions
	 */
	function savepointCreate($id) {
		$this->Query('SAVEPOINT');
	}

	function savepointRelease($id) {
		$this->Query('RELEASE SAVEPOINT '.$id);
	}

	function savepointRollbackTo($id) {
		$this->Query('ROLLBACK TO '.$id);
	}


	function exportFloat($string) {
		return str_replace(',', '.', $string);
	}

	function importBoolean($string) {
		return $string == 't' || $string == '1';
	}

	function DateTimeString($value) {
		$matches = array();
		eregi("^([[:digit:]]*).([[:digit:]]*).([[:digit:]]*) ([[:digit:]]*):([[:digit:]]*):([[:digit:]]*)", $value, $matches);
		$day    = $matches[3];
		$month  = $matches[2];
		$year   = $matches[1];
		$hour   = $matches[4];
		$minute = $matches[5];
		$second = $matches[6];

		return $day.'.'.$month.'.'.$year.' '.$hour.':'.$minute.':'.$second;
	}

	function importDateTime($string) {
		return $this->DateTimeString($string);
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

		$result = $this->Query($query, TRUE);
		return (is_array($result) ? (int)current(current($result)) : 0);
	}

	function loggerSqlFormat() {
		return false;
	}
}
?>
