<?php

namespace PP\Lib\Database\Driver;

use PP\Lib\Cache\CacheInterface;
use PP\Lib\Database\AbstractSqlDatabase;

/**
 * Class PostgreSqlDriver
 * @package PP\Lib\Database\Driver
 */
class PostgreSqlDriver extends AbstractSqlDatabase
{

	final public const TYPE = 'pgsql';

	public $connectString;
	public $connection;

	/** @var CacheInterface */
	public $cache;

	/** @var bool */
	public $connected;

	protected $includeOptions = ['connect_timeout' => ''];
	protected $excludeOptions = ['encoding'];

	protected $insideTransaction = false;
	protected $transactionsStack = [];

	/**
	 * @param \NLDBDescription $dbDescription
	 */
	public function __construct($dbDescription)
	{
		$this->setCache($dbDescription->cache);
		$this->connectArray = array_merge($this->connectArray, $this->includeOptions);
		$this->setConnectParams($dbDescription);
		$this->connected = false;
	}

	public function setConnectParams($dbDescription)
	{
		$keys = array_keys($this->connectArray);
		$connectString = [];

		foreach ($keys as $key) {
			if ($dbDescription->$key !== null) {
				$this->connectArray[$key] = $dbDescription->$key; // TODO: refactor
				if (!in_array($key, $this->excludeOptions)) {
					$connectString[] = $key . '=' . $dbDescription->$key; // TODO: refactor
				}
			}
		}

		$this->connectString = trim(implode(' ', $connectString));
	}

	public function Connect()
	{
		if (!$this->connected) {
			$this->connection = pg_connect($this->connectString, PGSQL_CONNECT_FORCE_NEW) or FatalError('Can\'t connect to ' . $this->connectString);

			if ($this->connection) {
				pg_exec($this->connection, "SET DATESTYLE='German'");

				if (!empty($this->connectArray['encoding'])) {
					pg_set_client_encoding($this->connection, $this->connectArray['encoding']);
				}

				$this->connected = true;
			}
		}
		return $this->connected;
	}

	public function Close()
	{
		if ($this->connected) {
			pg_close($this->connection);
			$this->connected = false;
		}
	}

	public function ModifyingQuery($query, $table = null, $retField = null, $flushCache = true, $retCount = false)
	{
		if (!$this->connected) {
			$this->Connect();
		}

		if ($this->connected) {
			if ($retField) {
				$retField = $this->EscapeString($retField);
				$query .= " RETURNING {$retField}";
			}

			if (($res = pg_query($this->connection, $query)) == false) {
				return ERROR_DB_BADQUERY;
			}

			if ($flushCache == true) {
				$this->cache->clear(); // TODO: refactor
			}

			if ($table && $retField) {
				if ($returnResult = pg_fetch_result($res, 0, $retField)) {
					return $returnResult;
				} else {
					return null;
				}
			} elseif ($retCount) {
				return pg_affected_rows($res);
			} else {
				return $res;
			}

		} else {
			return ERROR_DB_CANNOTCONNECT;
		}
	}

	public function ModifyingCopy($tableName, $cols, $data)
	{
		if (!$this->connected) {
			$this->Connect();
		}

		$row = reset($data);
		if (empty($row)) {
			return ERROR_DB_BADQUERY;
		}

		if ($this->connected) {

			$query = "COPY {$tableName} (\"" . implode('", "', $cols) . "\") FROM stdin";

			if (pg_query($this->connection, $query) == false) {
				return ERROR_DB_BADQUERY;
			}

			$from = ["\\", "\r", "\n", "\t", "\1"];
			$to = ["\\\\", "\\r", "\\n", "\\t", "\t"];
			$lines = [];

			do {
				$line = str_replace($from, $to, implode("\1", $row));

				while (str_contains($line, "\t\t")) {
					$line = str_replace("\t\t", "\t\\N\t", $line);
				}

				if ($line[0] == "\t") {
					$line = "\\N" . $line;
				}

				if ($line[strlen($line) - 1] == "\t") {
					$line .= "\\N";
				}

				$lines[] = $line;

			} while ($row = next($data));

			if (pg_put_line($this->connection, join("\n", $lines) . "\n") == false) {
				return ERROR_DB_BADQUERY;
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

	public function Query($query, $donotusecache = false, $limitpair = null)
	{
		if (is_array($limitpair)) {
			$limitstring = " LIMIT {$limitpair[0]} OFFSET {$limitpair[1]}";
		} else {
			$limitstring = "";
		}

		$query .= $limitstring;

		if (!is_null($table = $this->_loadFromCache($query, $donotusecache))) {
			return $table;
		}

		if (!$this->connected) {
			$this->Connect();
		}

		if ($this->connected) {
			if (($res = pg_query($this->connection, $query)) === false) {
				return ERROR_DB_BADQUERY;
			}

			$table = [];
			$total = pg_num_rows($res);

			for ($i = 0; $i < $total; $i++) {
				$table[] = pg_fetch_assoc($res, $i);
			}

			$this->_saveToCache($query, $table, $donotusecache);
		} else {
			return ERROR_DB_CANNOTCONNECT;
		}

		return $table;
	}

	public function InsertObject($table, $fields, $values, $flushCache = true)
	{
		$query = "INSERT INTO {$table} (\"" . implode('", "', $fields) . "\") VALUES (" . implode(', ', array_map($this->__mapInsertData(...), $values)) . ")";
		return $this->ModifyingQuery($query, $table, 'id', $flushCache);
	}

	public function MapData($value)
	{
		return match (true) {
			is_null($value) || $value === '' => "NULL",
			$value === "##now##" || $value === "now()" => "now()",
			is_bool($value) => $value ? "'t'" : "'f'",
			default => "'" . $this->EscapeString($value) . "'",
		};
	}

	public function mapFields($field)
	{
		return '"' . $this->EscapeString($field) . '"';
	}

	public function __mapInsertData($value)
	{
		return $this->MapData($value);
	}

	public function __mapUpdateData($field, $value)
	{
		return "\"{$field}\" = " . $this->MapData($value);
	}

	public function EscapeString($s)
	{
		return pg_escape_string($s);
	}

	public function UpdateObjectById($table, $objectId, $fields, $values, $flushCache = true)
	{
		$query = "UPDATE {$table} SET " . implode(', ', array_map($this->__mapUpdateData(...), $fields, $values)) . " WHERE id={$objectId}";
		return $this->ModifyingQuery($query, null, null, $flushCache);
	}


	/**
	 * @return $this
	 */
	public function transactionBegin()
	{
		if (!$this->insideTransaction) {
			$this->Query('BEGIN', true);
			$this->insideTransaction = true;
		} else { // emulating nested transactions
			$savepointId = uniqid('px_');
			$this->savepointCreate($savepointId);
			$this->transactionsStack[] = $savepointId;
		}
		return $this;
	}

	/**
	 * @return $this
	 */
	public function transactionCommit()
	{
		if (empty($this->transactionsStack)) {
			$this->Query('END', true);
			$this->insideTransaction = false;
		} else { // emulating nested transactions
			$savepointId = array_pop($this->transactionsStack);
			$this->savepointRelease($savepointId);
		}
		return $this;
	}

	public function transactionRollback()
	{
		if (empty($this->transactionsStack)) {
			$this->Query('ROLLBACK', true);
			$this->insideTransaction = false;
		} else { // emulating nested transactions
			$savepointId = array_pop($this->transactionsStack);
			$this->savepointRollbackTo($savepointId);
		}
	}

	/**
	 * Named transactions (savepoints) related functions
	 */
	public function savepointCreate($id)
	{
		$this->Query('SAVEPOINT ' . $id, true);
	}

	public function savepointRelease($id)
	{
		$this->Query('RELEASE SAVEPOINT ' . $id, true);
	}

	public function savepointRollbackTo($id)
	{
		$this->Query('ROLLBACK TO ' . $id, true);
	}


	public function importDateTime($string)
	{
		return $string;
	}

	public function exportFloat($string)
	{
		return str_replace(',', '.', (string) $string);
	}

	public function exportDateTime($string)
	{
		return $string == '00.00. 00:00:00' ? null : $string;
	}

	public function importBoolean($string)
	{
		return $string == 't' || $string == '1';
	}

	public function IsUniqueColsCombination($tables, $colValues, $exception)
	{
		if (!is_array($tables) || !sizeof($tables)) {
			FatalError("Вы не указали проверяемые таблицы");
		}

		if (!is_array($colValues) || !sizeof($colValues)) {
			FatalError("Вы не указали проверяемые столбцы");
		}

		$query = "SELECT (0 ";
		foreach ($tables as $t) {
			[$where_clauses, $tableName] = [[], $t['tableName']];

			foreach ($colValues as $c => $v) {
				$where_clauses[] = sprintf("%s = '%s'", $c, $v);
			}

			if (sizeof($exception)) {
				foreach ($exception as $c => $v) {
					$where_clauses[] = sprintf("%s != '%s'", $c, $v);
				}
			}

			$query = sprintf("%s + (SELECT count(*) FROM %s WHERE %s",
				$query, $tableName, join(" AND ", $where_clauses));

			if ($t['exWhere']) {
				$query = sprintf("%s and %s", $query, $t['exWhere']);
			}

			$query .= ")";
		}
		$query .= ")";

		return (int)(current(current($this->Query($query, true)))); // TODO: refactor
	}

	public function getError()
	{
		return pg_last_error();
	}

	public function tableExists($tableName)
	{
		return is_countable($this->query("SELECT relname FROM pg_class WHERE relname='{$tableName}'")) ? count($this->query("SELECT relname FROM pg_class WHERE relname='{$tableName}'")) : 0;
	}

	public function LIKE($condition, $percs)
	{
		return $this->_searchMethod("ILIKE", $condition, $percs);
	}

	public function inArray($arrayField, $value, $sane = false)
	{
		return sprintf("%s @> ARRAY[%s]", $sane ? $arrayField : $this->EscapeString($arrayField), $sane ? $value : $this->EscapeString($value));
	}

	public function arrayIn($arrayField, $value, $sane = false)
	{
		return sprintf("%s <@ ARRAY[%s]", $sane ? $arrayField : $this->EscapeString($arrayField), $sane ? $value : $this->EscapeString($value));
	}

	public function intersectIntArray($arrayField, $values)
	{
		return sprintf("%s && '{%s}'",
			$this->EscapeString($arrayField),
			join(",", array_filter($values, "is_numeric")));
	}

	public function ifStatement($when, $then, $else = null)
	{
		$else = $else ? "ELSE ({$else})" : '';
		return sprintf("CASE WHEN (%s) THEN (%s) %s END", $when, $then, $else);
	}

	public function caseStatement($arr, $else = null)
	{
		$else = $else ? "ELSE ({$else})" : '';
		$whenthens = '';
		foreach ((array)$arr as $when => $then) {
			$whenthens .= sprintf('WHEN (%s) THEN (%s) ', $when, $then);
		}
		return sprintf("CASE %s %s END", $whenthens, $else);
	}

	public function vacuumTable($tableName)
	{
		$this->Query("VACUUM " . $tableName, true);
		$this->Query("VACUUM ANALYZE " . $tableName, true);
	}

	public function loggerSqlFormat($table, $fields)
	{
		if (!(is_countable($fields) ? count($fields) : 0)) return false;
		$fieldNames = implode(', ', array_map($this->mapFields(...), array_keys($fields)));
		$fieldValues = implode(', ', array_map($this->MapData(...), array_values($fields)));
		return sprintf("INSERT INTO %s (%s) VALUES(%s)", $table, $fieldNames, $fieldValues);
	}
}
