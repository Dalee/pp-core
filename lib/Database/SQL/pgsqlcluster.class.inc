<?
/*
База становится плохой если мы не смогли записать в нее или прочитать из нее.
При этом создается файлик.
Из плохой базы мы НИКОГДА не читаем.
База становится хорошей при ModifyingQuery если удалось законнектиться.
При этом файлик удаляется.
*/
require_once 'pgsql.class.inc';
require_once 'abstract.class.inc';

class NLPGSQLClusterDatabase extends NLPGSQLDatabase {
	var $statusDir;
	var $cluster;

	function NLPGSQLClusterDatabase($dbDescription, $statusDir) {
		$this->statusDir = $statusDir;
		$this->cluster   = array();
		foreach ($dbDescription as $k=>$v) {
			$this->cluster[] = new NLPGSQLDatabase($v);
		}
	}

	function ModifyingQuery($query, $table=NULL, $retField=NULL, $flushCache = true) {
		foreach ($this->cluster as $k=>$db) {
			if ($this->IsGood($k)) {
				$result = $db->ModifyingQuery($query, $table, $retField, $flushCache);
				if ($result == ERROR_DB_CANNOTCONNECT) {
					$this->MakeBad($k);
					AppendStringToFile($this->statusDir."/{$k}.dat", $query.";\n");
				} else {
					$goodResult = $result;
				}
			} else {
				$db->Connect();
				if ($db->connected) {
					$this->FlushRecords($k);
					$goodResult = $db->ModifyingQuery($query, $table, $retField, $flushCache);
				} else {
					AppendStringToFile($this->statusDir."/{$k}.dat", $query.";\n");
				}
			}
		}
		return $goodResult;
	}

	function Query($query) {
		foreach ($this->cluster as $k=>$db) {
			if (!$this->IsGood($k)) continue;
			$table = $db->Query($query);
			if (is_array($table)) {
				return $table;
			} else {
				if ($table == ERROR_DB_CANNOTCONNECT) {
					$this->MakeBad($k);
				}
				return $table;
			}
		}
		return ERROR_DB_CANNOTCONNECT;
	}

	function MakeBad($dbKey) {
		// tmp
		return;
		WriteStringToFile($this->statusDir."/{$dbKey}.dat", "");
	}

	function MakeGood($dbKey) {
		unlink($this->statusDir."/{$dbKey}.dat");
	}

	function IsGood($dbKey) {
		return !file_exists($this->statusDir."/{$dbKey}.dat");
	}

	function FlushRecords($dbKey) {
		if (file_exists($this->statusDir."/{$dbKey}.dat")) {
			$this->cluster[$dbKey]->Connect();
			pg_send_query($this->cluster[$dbKey]->connection, ReadFileToString($this->statusDir."/{$dbKey}.dat"));
			while (pg_connection_busy($this->cluster[$dbKey]->connection));
			unlink(BASEPATH."/var/dbstatus/{$dbKey}.dat");
		}
	}

}

?>