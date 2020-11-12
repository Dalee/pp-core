<?php

namespace PP\Lib\Database;

use PP\Lib\Cache\ObjectCache;

/**
 * Class AbstractSqlDatabase
 * @package PP\Lib\Database
 */
class AbstractSqlDatabase
{

	/** @var ObjectCache */
	public $cache = null;

	protected $connectArray = [
		'host' => 'localhost',
		'user' => '',
		'port' => '',
		'password' => '',
		'dbname' => 'test',
		'encoding' => DEFAULT_CHARSET,
	];

	public function __construct(\NLDBDescription $dbDescription)
	{
	}

	public function connect()
	{
	}

	public function close()
	{
	}

	public function setCache($cacheType)
	{
		$this->cache = ObjectCache::get($cacheType, 'database');
	}

	public function modifyingQuery($query, $table = null, $retField = null, $flushCache = true, $retCount = false)
	{
	}

	public function modifyingCopy($tableName, $cols, $data)
	{
	}

	public function query($query, $donotusecache = false, $limitpair = null)
	{
	}

	public function insertObject($table, $fields, $values)
	{
	}

	public function limitOffsetString($limit, $offset)
	{
	}

	public function trueStatusString($status = 'TRUE')
	{
		return ($status == 'TRUE' || $status == 1) ? 'TRUE' : 'FALSE';
	}

	public function updateObjectById($table, $objectId, $fields, $values)
	{
	}

	public function dateTimeString($string)
	{
	}

	public function isUniqueColsCombination($tables, $colValues, $exception)
	{
	}

	public function getTableInfo($tableName)
	{
		return [];
	}

	public function getError()
	{
		return "Error!";
	}

	public function EscapeString($string)
	{
		return addslashes($string);
	}

	public function mapFields($field)
	{
		return $field;
	}

	public function exportFloat($value)
	{
		return $value;
	}

	public function exportDateTime($value)
	{
		return $value;
	}

	public function vacuumTable($tableName)
	{
	}

	public function tableExists($tableName)
	{
		return true;
	}

	public function transactionBegin()
	{
	}

	public function transactionCommit()
	{
	}

	public function transactionRollback()
	{
	}

	public function savepointCreate($name)
	{
	}

	public function savepointRelease($name)
	{
	}

	public function savepointRollbackTo($name)
	{
	}

	public function LIKE($condition, $percs)
	{
		return $this->_searchMethod("LIKE", $condition, $percs);
	}

	public function inArray($arrayField, $value, $sane = false)
	{
	}

	public function intersectIntArray($arrayField, $values)
	{
	}

	public function _searchMethod($meth, $condition, $percs)
	{
		$lperc = P_LEFT & $percs ? '%' : '';
		$rperc = P_RIGHT & $percs ? '%' : '';
		return " " . $meth . " '" . $lperc . $this->EscapeString($condition) . $rperc . "' ";
	}

	public function _loadFromCache($query, $customCacheExpiration)
	{
		if (!$customCacheExpiration || (is_int($customCacheExpiration) && $customCacheExpiration > 0)) {
			$data = $this->cache->load($query);
			if ($data !== null) {
				return $data;
			}
		}
	}

	public function _saveToCache($query, $data, $customCacheExpiration)
	{
		if (!$customCacheExpiration) {
			$this->cache->save($query, $data);
		} elseif (is_int($customCacheExpiration) && $customCacheExpiration > 0) {
			$this->cache->save($query, $data, $customCacheExpiration);
		}
	}

	public function loggerSqlFormat($table, $fields)
	{

	}
}
