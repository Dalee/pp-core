<?
require_once('dbdescription.class.inc');
require_once('abstractsql.class.inc');

class NLMYSQLDatabase extends NLAbstractSQLDatabase {
	var $connectArray;
	var $connection;
	var $cache;
	var $connected;

	function NLMYSQLDatabase($dbDescription) {
		$this->connectArray   = $dbDescription->GetMysqlConnectArray();
		$this->cache          = $dbDescription->cache;
		$this->connected      = false;
		$this->dbids          = $dbDescription->dbids;
	}

	function Connect() {
		if (!$this->connected) {
			$this->connection = @mysql_connect($this->connectArray['host'].':'.$this->connectArray['port'], $this->connectArray['user'], $this->connectArray['passwd']);

			if ($this->connection) {
				if (@mysql_select_db($this->connectArray['dbname'], $this->connection)) {
					$this->connected  = true;
				}
			}
		}
	}

	function Close() {
		if ($this->connected) {
			mysql_close($this->connection);
			$this->connected = false;
		}
	}

	function ModifyingQuery($query, $table=NULL, $retField=NULL, $flushCache = true) {
		if (!$this->connected) $this->Connect();
		if ($this->connected) {
			if (($res = mysql_query($query, $this->connection)) == false) {
				return ERROR_DB_BADQUERY;
			}
			if ($this->cache == true && $flushCache == true) {
				$d = new NLDir(BASEPATH . DIRECTORY_SEPARATOR . 'tmp/cache/db');
				$d->Emptyfy();
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

	function Query($query, $donotusecache = false) {
		$profileIden = "SQL: ".$query;
		NLProfiler($profileIden, "BEGIN");
        
        $full_filename = BASEPATH . DIRECTORY_SEPARATOR . 'tmp/cache/db/' . md5($query);

		if ($this->cache == true && !$donotusecache && file_exists($full_filename)) {
			$table = unserialize(ReadFileToString($full_filename));
			NLProfiler($profileIden, "END");
			return $table;
		}

		if (!$this->connected) $this->Connect();

		if ($this->connected) {
			if (($res = @mysql_query($query, $this->connection)) == false) {
				NLProfiler($profileIden, "END");
				return ERROR_DB_BADQUERY;
			}
			$table = array();
			while (($row = mysql_fetch_array($res, MYSQL_ASSOC))) {
				$table[] = $row;
			}
			mysql_free_result($res);

			if ($this->cache == true && !$donotusecache) {
				WriteStringToFile($full_filename, serialize($table));
			}
		} else {
			NLProfiler($profileIden, "END");
			return ERROR_DB_CANNOTCONNECT;
		}

		NLProfiler($profileIden, "END");
		return $table;
	}

	function InsertObject($table, $fields, $values) {
		foreach ($values as $k=>$v) {
			$values[$k] = addslashes($v);
		}
		$nextID = $this->GetNextID();
		$query = "INSERT INTO $table (id, ";
		if(!in_array('sys_order', $fields)) {
			$query .= "sys_order, ";
		}
		$query  .= implode(", ", $fields).") VALUES (".$nextID;

		if(!in_array('sys_order', $fields)) {
			$query .= ", ".$nextID;
		}
		$query  .= ", ".str_replace("''", "NULL", "'".implode("', '", $values)."'").")";

		$query  = str_replace("'now()'", "now()", $query);
		$id     = $this->ModifyingQuery($query, $table, 'id');
		$id     = $nextID;
		return $id;
	}

	function GetNextID() {
		if (!$this->connected) $this->Connect();
		$res = mysql_query("SELECT id FROM ".$this->dbids, $this->connection);
		$lastID = mysql_result($res, 'id');
		$nextID = $lastID + 1;
		mysql_query("UPDATE ".$this->dbids." SET id = ".$nextID, $this->connection);
		return $nextID;
	}

	function LimitOffsetString($limit, $offset) {
		return "LIMIT {$offset}, {$limit}";
	}

	function TrueStatusString($status) {
		return ($status == 'TRUE' || $status == 1) ? '1' : '0';
	}

	function UpdateObjectById($table, $objectId, $fields, $values) {
		$updateString = NULL;
		foreach ($fields as $k=>$v) {
			if ($v != 'id') {
				if (is_null($values[$k])) {
					$updateString .= $v ." = NULL, ";
				} else {
					$updateString .= $v ." = '".addslashes($values[$k])."', ";
				}
			}
		}
		$updateString = substr($updateString, 0, -2);
		$query = "UPDATE $table SET $updateString WHERE id=$objectId";
		$this->ModifyingQuery($query);
		return NULL;
	}

	function TransactionString($begin=TRUE) {
		return $begin ? 'START TRANSACTION' : 'COMMIT';
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
		if(!is_array($tables) || !sizeof($tables)) {
			FatalError("Вы не указали проверяемые таблицы");
		}

		if(!is_array($colValues) || !sizeof($colValues)) {
			FatalError("Вы не указали проверяемые столбцы");
		}

		$reiteration = 0;

		foreach($tables as $t) {
			$query = "SELECT count(*) FROM {$t} WHERE ";

			foreach($colValues as $c=>$v) {
				$query .= "{$c} = '{$v}' AND ";
			}

			if(sizeof($exception)) {
				foreach($exception as $c=>$v) {
					$query .= "{$c} != '{$v}' AND ";
				}
			}

			$query = substr($query, 0, -4);
			$reiteration += (int)current(current($this->Query($query, TRUE)));
		}

		return $reiteration;
	}
}

?>
