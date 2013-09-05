<?php
	/**
	 * sqlite
	 * dbname = /path/to/database.sqlite3
	 * 
	 */
	require_once (PPLIBPATH.'Database/SQL/abstract.class.inc');

	function _sqlite_split_part ($value, $delimiter, $partNumber) {
		$partNumber = intval($partNumber) - 1; // 1-based in psql
		$res = explode($delimiter, $value);
		return (count($res) > $partNumber) ? $res[$partNumber] : "";
	}

	class NLSQLiteDatabase extends NLAbstractSQLDatabase {
		/** @var PDO */
		protected $connection = null;
		protected $allowedOptions = array('dbname');
		private $_internalEncoding = 'utf-8';

		public function __construct($dbDescription) {
			$this->setCache($dbDescription->cache);
			$this->setConnectParams($dbDescription);
			$this->_check();
		}

		protected function _check () {
			if (!class_exists('PDO')) {
				FatalError('PDO support is not enabled');
			}
			if (!in_array('sqlite', PDO::getAvailableDrivers())) {
				FatalError('PDO sqlite driver support is not enabled');
			}
			if (empty($this->connectArray['dbname'])) {
				FatalError('SQLite database file is not set');
			}
		}

		protected function setConnectParams($dbDescription) {
			foreach(array_keys($this->connectArray) as $field) {
				if ($dbDescription->$field !== null) {
					$this->connectArray[$field] = $dbDescription->$field;
				}
			}
		}

		public function connect () {
			$databaseFile = $this->connectArray['dbname'];
			$databaseFile = rtrim(BASEPATH, '/').'/tmp/'.$databaseFile;

			if (!file_exists($databaseFile)) {
				if (!touch($databaseFile)) {
					FatalError('Unable to create sqlite database file');
				}
			}
			try {
				$dsn = "sqlite://{$databaseFile}";
				$this->connection = new PDO($dsn);

				// emulate some functionality
				$this->connection->sqliteCreateFunction('split_part', '_sqlite_split_part', 3);
				

			} catch(PDOException $e) {
				$this->connection = null;
				FatalError(sprintf("SQLite connection failed: %s", $e->getMessage()));
			}

			return ($this->connection !== null);
		}

		public function close () {
			if ($this->connection) {
				$this->connection = null;
			}
		}

		public function getError() {
			$ret = 'Not connected';
			if ($this->connection) {
				$errorInfo = $this->connection->errorInfo();
				$ret = implode(', ', $errorInfo);
			}
			return $ret;
		}

		public function ModifyingQuery($query, $table = null, $retField = null, $flushCache = true, $retCount = false) {
			d2(func_get_args());
			PXProfiler::begin('SQL: '.$query);
			if (!$this->connection) {
				$this->connect();
				if (!$this->connection) {
					PXProfiler::end();
					return ERROR_DB_CANNOTCONNECT;
				}
			}

			/** @var PDOStatement $stmt */
			$stmt = null;
			if (!($stmt = $this->connection->query($query))) {
				PXProfiler::end();
				return ERROR_DB_BADQUERY;
			}

			if ($flushCache == true) {
				$this->cache->clear();
			}

			if ($table && $retField) {
				$lastInsertId = $this->connection->lastInsertId();
				$stmt = null;
				$table = $this->Query("SELECT {$retField} FROM {$table} WHERE id = {$lastInsertId}", true);

				if (is_array($table) && count($table)) {
					PXProfiler::end();
					return $table[0][$retField];
				}
				PXProfiler::end();
				return null;
			}

			if ($retCount) {
				$rowCount = $stmt->rowCount();
				$stmt = null;
				return $rowCount;
			}

			PXProfiler::end();
			return ($stmt != null) ? 1 : 0;
		}

		public function Query($query, $donotusecache = false, $limitpair = NULL) {
			$query .= (is_array($limitpair)) ? " LIMIT {$limitpair[0]} OFFSET {$limitpair[1]}" : "";

			PXProfiler::begin('SQL: '.$query);
			if (!is_null($table = $this->_loadFromCache($query, $donotusecache))) {
				PXProfiler::end();
				return $table;
			}

			if (!$this->connection) {
				$this->connect();
				if (!$this->connection) {
					PXProfiler::end();
					return ERROR_DB_CANNOTCONNECT;
				}
			}

			/** @var PDOStatement $stmt */
			$stmt = null;
			if (!($stmt = $this->connection->query($query))) {
				PXProfiler::end();
				return ERROR_DB_BADQUERY;
			}

			$table = array();
			while (($row = $stmt->fetch(PDO::FETCH_ASSOC))) {
				$realRow = array();

				foreach ($row as $key => $value) {
					if (!is_null($value)) {
						$value = $this->UnescapeString($value);

						if (strcasecmp(DEFAULT_CHARSET, $this->_internalEncoding)) {
							$value = myconv($this->_internalEncoding, DEFAULT_CHARSET, $value);
						}
					}

					$key = trim($key, '"');
					$realRow[$key] = $value;
				}
				$table[] = $realRow;
			}

			$this->_saveToCache($query, $table, $donotusecache);
			PXProfiler::end();
			return $table;
		}

		public function InsertObject($table, $fields, $values) {
			$fields = array_map(array($this, 'mapFields'), $fields);
			$query = "INSERT INTO {$table} (".implode(', ', $fields).") VALUES (".implode(', ', array_map(array($this, '__mapInsertData'), $values)).")";
			$id    = $this->ModifyingQuery($query, $table, 'id');
			return $id;
		}

		public function UpdateObjectById($table, $objectId, $fields, $values) {
			$query = "UPDATE {$table} SET ".implode(', ', array_map(array($this, '__mapUpdateData'), $fields, $values))." WHERE id={$objectId}";
			return $this->ModifyingQuery($query, $table, 'id');
		}


		public function MapData($value) {
			$ret = "__ERROR__";
			switch(true) {
				case is_null($value) || $value === '' :
					$ret = "NULL";
					break;

				case $value === "##now##" || $value === "now()":
					$ret = "CURRENT_TIMESTAMP";
					break;

				case is_bool($value):
					$ret = ($value) ? "'1'" : "'0'";
					break;
				default:
					$ret = "'".$this->EscapeString($value)."'";
					break;
			}

			if (strcasecmp(DEFAULT_CHARSET, $this->_internalEncoding)) {
				$ret = myconv(DEFAULT_CHARSET, $this->_internalEncoding, $ret);
			}
			return $ret;
		}

		// multiregions driver
		public function inArray($arrayField, $value, $sane = false) {
			// TODO: refactor to user-defined function
			return sprintf("%s @> ARRAY[%s]", $sane ? $arrayField : $this->EscapeString($arrayField), $sane ? $value : $this->EscapeString($value));
		}
		
		public function arrayIn($arrayField, $value, $sane = false) {
			// TODO: refactor to user-defined function
			return sprintf("%s <@ ARRAY[%s]", $sane ? $arrayField : $this->EscapeString($arrayField), $sane ? $value : $this->EscapeString($value));
		}

		public function intersectIntArray($arrayField, $values) {
			// TODO: refactor to user-defined function
			return sprintf("%s && '{%s}'",
				$this->EscapeString($arrayField),
				join(",", array_filter($values, "is_numeric")));
		}

		// helpers
		public function tableExists($tableName) {
			$tableName = $this->MapData($tableName);
			return count($this->query("select name from sqlite_master where name={$tableName} and type='table'"));
		}
	
		public function EscapeString($string) {
			return addslashes($string);
		}

		public function UnescapeString($string) {
			return stripslashes($string);
		}

		public function importBoolean($string) {
			return $string == '1' || $string == 't';
		}

		public function importDateTime($string) {
			return $string;
		}

		// mappers 
		public function mapFields($field) {
			return '"'.$this->EscapeString($field).'"';
		}

		private function __mapInsertData($value) {
			return $this->MapData($value);
		}

		private function __mapUpdateData($field, $value) {
			return $this->mapFields($field)." = ".$this->MapData($value);
		}

		// logger helper
		public function loggerSqlFormat($table, $fields) {
			if (!count($fields)) return false;
			$fieldNames = implode(', ', array_map(array($this, "mapFields"), array_keys($fields)));
			$fieldValues = implode(', ', array_map(array($this, "MapData"), array_values($fields)));
			return sprintf("INSERT INTO %s (%s) VALUES(%s)", $table, $fieldNames, $fieldValues);
		}

		//todo: add transactions and savepoints (http://www.sqlite.org/lang_transaction.html http://www.sqlite.org/lang_savepoint.html)
	}
?>
