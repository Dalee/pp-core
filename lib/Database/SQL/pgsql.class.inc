<?php
require_once('abstract.class.inc');

class NLPGSQLDatabase extends NLAbstractSQLDatabase {

	var $connectString;
	var $connection;
	var $cache;
	var $encoding;
	var $connected;

	function NLPGSQLDatabase($dbDescription) {
		$this->connectString   = $dbDescription->GetPgsqlConnectString();
		$this->cache           = $dbDescription->cache;
		$this->encoding        = $dbDescription->encoding;
		$this->connected       = false;
	}

	function Connect() {
		if (!$this->connected) {
			$this->connection = pg_connect($this->connectString);
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
		if (!$this->connected) $this->Connect();
		if ($this->connected) {
			if (($res = pg_query($this->connection, $query)) == false) {
				return ERROR_DB_BADQUERY;
			}
			if ($this->cache == true && $flushCache == true) {
				$d = new NLDir(BASEPATH. DIRECTORY_SEPARATOR . 'tmp/cache/db');
				$d->Emptyfy();
			}
			if ($table && $retField) {
				$table = $this->Query("SELECT {$retField} FROM {$table} WHERE oid = ".pg_last_oid($res), true);
				if (is_array($table) && count($table)) {
					return $table[0][$retField];
				} else {
					return NULL;
				}
			} else {
				return $res;
			}
		} else {
			return ERROR_DB_CANNOTCONNECT;
		}
	}

	function ModifyingCopy($tableName, $cols, &$data) {
		if (!$this->connected) $this->Connect();
		if ($this->connected) {
			$query = "COPY {$tableName} (\"".implode('", "', $cols)."\") FROM stdin";
			if (pg_query($this->connection, $query) == false) {
				return ERROR_DB_BADQUERY;
			}

			foreach ($data as $k=>$row) {
				$line = implode("----qweeqwe----", $row);
				$line = str_replace("\\", "\\\\", $line);
				$line = str_replace("\r", "\\r", $line);
				$line = str_replace("\n", "\\n", $line);
				$line = str_replace("\t", "\\t", $line);
				$line = str_replace("----qweeqwe----", "\t", $line);
				while (strstr($line, "\t\t")) $line = str_replace("\t\t", "\t\\N\t", $line);
				if ($line{strlen($line)-1} == "\t") $line .= "\\N";
				if (pg_put_line($this->connection, $line."\n") == false) {
					return ERROR_DB_BADQUERY;
				}
			}
			if (pg_put_line($this->connection, "\\.\n") == false) {
				return ERROR_DB_BADQUERY;
			}
			if (pg_end_copy($this->connection) == false) {
				return ERROR_DB_BADQUERY;
			}
		} else {
			return ERROR_DB_CANNOTCONNECT;
		}

		return true;
	}

	function Query($query, $donotusecache = false, $limitpair = NULL) {
		if (is_array($limitpair)) {
			$limitstring = " LIMIT {$limitpair[0]} OFFSET {$limitpair[1]}";
		} else {
			$limitstring = "";
		}
		$query .= $limitstring;

		$profileIden = "SQL: ".$query;
		NLProfiler($profileIden, "BEGIN");

		if ($this->cache == true && !$donotusecache) {
			$filename = BASEPATH . DIRECTORY_SEPARATOR . 'tmp/cache/db/'.md5($query);
			$fp = @fopen($filename, 'rb');
			if ($fp !== false && flock($fp, LOCK_SH)) {
				if (($fsize = filesize($filename)) > 0) {
					$str = fread($fp, $fsize);
					if ($str !== '') {
						if (($ret = @unserialize($str)) !== false) {
							NLProfiler($profileIden, "END");
							return $ret;
						}
					}
				}
				flock($fp, LOCK_UN);
				fclose($fp);
			}
		}
		if (!$this->connected) $this->Connect();
		if ($this->connected) {
			if (($res = pg_query($this->connection, $query)) === false) {
				NLProfiler($profileIden, "END");
				return ERROR_DB_BADQUERY;
			}
			$table = array();
			$total = pg_num_rows($res);
			for ($i=0; $i<$total; $i++) {
				$table[] = pg_fetch_assoc($res, $i);
			}
			if ($this->cache == true && !$donotusecache) {
				$d = new NLDir(BASEPATH. DIRECTORY_SEPARATOR . 'tmp/cache/db');
			    if (!$d->exist()) { 
                    $d->create();
                }
                WriteStringToFile(BASEPATH.DIRECTORY_SEPARATOR.'tmp/cache/db/'.md5($query), serialize($table));
			}
		} else {
			NLProfiler($profileIden, "END");
			return ERROR_DB_CANNOTCONNECT;
		}
		NLProfiler($profileIden, "END");
		return $table;
	}


	function InsertObject($table, $fields, $values) {
		$query = "INSERT INTO {$table} (\"".implode('", "', $fields)."\") VALUES (".
				implode(', ', array_map(array(&$this, '__mapInsertData'), $values)).")";
		$id    = $this->ModifyingQuery($query, $table, 'id');
		return $id;
	}

	function MapData($value) {
		if (is_null($value) || $value === '') return "NULL";
		else if ($value === "##now##")        return "now()";
		else if ($value === "now()")          return "now()";
		else return "'".$this->EscapeString($value)."'";
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
		$query = "UPDATE {$table} SET ".implode(', ', array_map(array(&$this, '__mapUpdateData'), $fields, $values)).
				" WHERE id={$objectId}";
		return $this->ModifyingQuery($query);
		return NULL;
	}

	function TransactionString($begin=TRUE) {
		return $begin ? 'BEGIN' : 'END';
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
			$query .= "+ (SELECT count(*) FROM {$t} WHERE ";

			foreach($colValues as $c=>$v) {
				$query .= "{$c} = '{$v}' AND ";
			}

			if(sizeof($exception)) {
				foreach($exception as $c=>$v) {
					$query .= "{$c} != '{$v}' AND ";
				}
			}
			$query = substr($query, 0, -4);
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

}

?>
