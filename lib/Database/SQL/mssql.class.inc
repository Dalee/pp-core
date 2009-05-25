<?php
require_once 'abstract.class.inc';

class NLMSSQLDatabase extends NLAbstractSQLDatabase {
	var $connectArray;
	var $connection;
	var $cache;
	var $connected;
	var $tservercoding;

	function NLMSSQLDatabase($dbDescription) {
		$this->setCache($dbDescription->cache);

		$this->connectArray    = $dbDescription->GetMssqlConnectArray();
		$this->connected       = false;
		$this->servercoding    = (isset($_SERVER['SERVER_PORT']) ? 'w' : 'd');
	}

	function Connect() {
		if (!$this->connected) {
			$this->connection = mssql_connect(
				$this->connectArray['host'] . (!empty($this->connectArray['port']) ? ",".$this->connectArray['port'] : ''),
				$this->connectArray['user'],
				$this->connectArray['password'],
				true
			);

			if ($this->connection) {
				mssql_query("USE {$this->connectArray['dbname']}; SET DATEFORMAT dmy; SET NOCOUNT OFF", $this->connection);
				$this->connected  = true;
			}
		}
	}

	function Close() {
		if ($this->connected) {
			mssql_close($this->connection);
			$this->connected = false;
		}
	}

	function Query($query, $donotusecache = false, $limitpair = NULL, $xxxx=false) {
		$query = convert_cyr_string($query, 'k', $this->servercoding);

		PXProfiler::begin('SQL: '.$query);

		$md = (is_array($limitpair)) ? "-{$limitpair[0]}-{$limitpair[1]}" : "";

		if (!is_null($table = $this->_loadFromCache($query.$md, $donotusecache))) {
		        return $table;
		}
		
		if (!$this->connected) {
			$this->Connect();
		}

		if ($this->connected) {
			if (is_array($limitpair)) {
				$bquery = "declare @handle int; exec sp_cursoropen @handle OUT, '".$this->__quote($query)."', 1, 1; exec sp_cursorfetch @handle, 16, ".($limitpair[1] + 1).", {$limitpair[0]}; exec sp_cursorclose @handle";

				if (($res = mssql_query($bquery, $this->connection)) === false) {
					PXProfiler::end();
					return ERROR_DB_BADQUERY;
				}

				if (mssql_next_result($res) === false) {
					PXProfiler::end();
					return ERROR_DB_BADQUERY;
				}

			} else {
				if (($res = mssql_query($query, $this->connection)) === false) {
					PXProfiler::end();
					return ERROR_DB_BADQUERY;
				}
			}

			$table = array();
			$count = 0;

			if(is_resource($res)) {
				while ($tmp = mssql_fetch_assoc($res)) {
					foreach ($tmp as $k => $v) {
						if (is_string($v)) {
							if ($v === " ") $v = ''; // FIXME: Fix bug #26315 http://bugs.php.net/bug.php?id=26315
							// may be change to fetch_array
							$tmp[$k] = convert_cyr_string($v, $this->servercoding, 'k');
						}
					}

					$table[$count++] = $tmp;
				}
			}

			$this->_saveToCache($query.$md, $table, $donotusecache);
		} else {
			PXProfiler::end();
			return ERROR_DB_CANNOTCONNECT;
		}

		PXProfiler::end();

		return $table;
	}


	function ModifyingQuery($query, $table=NULL, $retField=NULL, $flushCache = true) {
		if (!$this->connected) {
			$this->Connect();
		}

		if ($this->connected) {
			$query = convert_cyr_string($query, 'k', $this->servercoding);
			ini_set('track_errors', 1);

			if (($res = mssql_query($query, $this->connection)) === false) {
				FatalError($query);
			}

			if(isset($php_errormsg)) {
				d2($query);
			}

			if ($flushCache == true) {
				$this->cache->clear();
			}

			if ($table && $retField) {
				$tmp = mssql_fetch_array($res);

				if (count($tmp) && !empty($tmp[0])) {
					$table = $this->Query("SELECT {$retField} FROM {$table} WHERE id = ".$tmp[0], true);

					if (is_array($table) && count($table)) {
						return $table[0][$retField];
					} else {
						return NULL;
					}
				} 
			}

			return $res;

		} else {
			return ERROR_DB_CANNOTCONNECT;
		}
	}

	function InsertObject($table, $fields, $values) {
		$query = "INSERT INTO {$table} (".implode(", ", array_map(array(&$this, '__mapFields'), $fields)).") VALUES (".implode(', ', array_map(array(&$this, '__mapInsertData'), $values))."); SELECT SCOPE_IDENTITY() AS s";
		$res = $this->ModifyingQuery($query, $table, 'id', true);
		return $res;
	}

	function __mapInsertData($value) {
		return $this->MapData($value);
	}

	function UpdateObjectById($table, $objectId, $fields, $values) {
		$query = "UPDATE {$table} SET ".implode(", ", array_map(array(&$this, '__mapUpdateData'), $fields, $values))." WHERE id={$objectId}";
		$this->ModifyingQuery($query);
		return NULL;
	}

	function MapData($value) {
		if (is_null($value) || $value === '') return "NULL";
		else if ($value === "##now##") return "GETDATE()";
		else if ($value === "now()")   return "GETDATE()";
		else return "'".$this->__quote($value)."'";
	}

	function __mapUpdateData($field, $value) {
		return $this->__mapFields($field)." = ".$this->MapData($value);
	}

	function __mapFields($field) {
		switch($field) {
			case 'index':
			case 'count':
				return "[".$field."]";

			default:
				return $field;
		}
	}

	function mapFields($field) {
		return $this->__mapFields($field);
	}

	function ModifyingCopy($tableName, $cols, $data) {
		if (!$this->connected) $this->Connect();
		if ($this->connected) {
			foreach ($data as $k=>$row) {
				$this->InsertObject($tableName, $cols, $row);
			}
		} else {
			return ERROR_DB_CANNOTCONNECT;
		}
	}

	function IsUniqueColsCombination($tables, $colValues, $exception) {
		if(!is_array($tables) || !sizeof($tables)) {
			FatalError("Вы не указали проверяемые таблицы");
		}

		if(!is_array($colValues) || !sizeof($colValues)) {
			FatalError("Вы не указали проверяемые столбцы");
		}

		$query = "SELECT (0 ";
		foreach($tables as $t) {
			$query .= "+ (SELECT COUNT(*) FROM {$t} WHERE ";

			foreach ($colValues as $c=>$v) {
				$query .= "{$c} = '{$v}' AND ";
			}

			if (sizeof($exception)) {
				foreach($exception as $c=>$v) {
					$query .= "{$c} != '{$v}' AND ";
				}
			}
			$query = substr($query, 0, -4);
			$query .= ")";
		}
		$query .= ")";
		return (int)(current(current($this->query($query, TRUE))));
	}

	/*
	 * Transaction related functions
	 *
	 */
	function transactionBegin() {
		$this->Query('BEGIN TRANSACTION');
	}

	function transactionCommit() {
		$this->Query('COMMIT TRANSACTION');
	}

	function transactionRollback() {
		$this->Query('ROLLBACK TRANSACTION');
	}

	function importDateTime($string) {
		$m = array();
		preg_match("/^(\d+)-(\d+)-(\d+) (\d+):(\d+):(\d+)/", $string, $m);

		if(sizeof($m) == 7) {
			return "{$m[3]}.{$m[2]}.{$m[1]} {$m[4]}:{$m[5]}:{$m[6]}";
		} else {
			//return "01.01.1900 00:00:00";
			return NULL;
		}
	}

	function importBoolean($string) {
		 return ($string == '1') ? true : false;
	}

	function exportDateTime($string) {
		$m = array();
		preg_match("/^(\d+)\.(\d+)\.(\d+) (\d+):(\d+):(\d+)/", $string, $m);

		if(sizeof($m) == 7) {
			return "{$m[1]}-{$m[2]}-{$m[3]} {$m[4]}:{$m[5]}:{$m[6]}";
		} else {
			return null;
		}
	}


	function __quote($s) {
		if (is_float($s)) {
			return str_replace(',', '.', strval($s));
		}
		return str_replace("'", "''", $s);
	}

	function EscapeString($s) {
		return $this->__quote($s);
	}

	function tableExists($tableName) {
		$a = $this->query("SELECT OBJECTPROPERTY(OBJECT_ID(N'{$tableName}'),N'IsUserTable')");

		if (count($a) && current(current($a)) == 1) {
			return true;
		}

		return false;
	}
}
?>