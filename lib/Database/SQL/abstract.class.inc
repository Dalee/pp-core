<?php

use PP\Lib\Cache\ObjectCache;

require_once PPLIBPATH . 'Config/Description/db.class.inc';

abstract class NLAbstractSQLDatabase {
	/** @var ObjectCache */
	public $cache = null;
	protected $connectArray = array(
		'host' => 'localhost',
		'user' => '',
		'port' => '',
		'password' => '',
		'dbname' => 'test',
		'encoding' => DEFAULT_CHARSET
	);

	public function __construct($dbDescription) {
	}

	function connect() {
	}

	function close() {
	}

	function setCache($cacheType) {
		$this->cache = ObjectCache::get($cacheType, 'database');
	}

	function modifyingQuery($query, $table=NULL, $retField=NULL, $flushCache = true, $retCount = false) {
	}

	function modifyingCopy($tableName, $cols, $data) {
	}

	function query($query, $donotusecache = false, $limitpair = NULL) {
	}

	function insertObject($table, $fields, $values) {
	}

	function limitOffsetString($limit, $offset) {
	}

	function trueStatusString($status='TRUE') {
		return ($status == 'TRUE' || $status == 1) ? 'TRUE' : 'FALSE';
	}

	function updateObjectById($table, $objectId, $fields, $values) {
	}

	function dateTimeString($string) {
	}

	function isUniqueColsCombination($tables, $colValues, $exception) {
	}

	function getTableInfo($tableName) {
		return array();
	}

	function getError() {
		return "Error!";
	}

	function EscapeString($string) {
		return addslashes($string);
	}

	function mapFields($field) {
		return $field;
	}

	function exportFloat($value) {
		return $value;
	}

	function exportDateTime($value) {
		return $value;
	}

	function vacuumTable($tableName) {
	}

	function tableExists($tableName) { /* E_STRICT */
		return true;
	}

	function transactionBegin() {
	}

	function transactionCommit() {
	}

	function transactionRollback() {
	}

	function savepointCreate($name) {
	}

	function savepointRelease($name) {
	}

	function savepointRollbackTo($name) {
	}

	function LIKE($condition, $percs){
		return $this->_searchMethod("LIKE", $condition, $percs);
	}

	function inArray($arrayField, $value, $sane = false){
	}

	function intersectIntArray($arrayField, $values){
	}

	function _searchMethod($meth, $condition, $percs){
		$lperc = P_LEFT  & $percs ? '%' : '';
		$rperc = P_RIGHT & $percs ? '%' : '';
		return " ".$meth." '".$lperc.$this->EscapeString($condition).$rperc."' ";
	}

	function _loadFromCache($query, $customCacheExpiration){
		if (!$customCacheExpiration || (is_int($customCacheExpiration) && $customCacheExpiration > 0)) {
			$data = $this->cache->load($query);
			if ($data !== null) {
				return $data;
			}
		}
	}

	function _saveToCache($query, $data, $customCacheExpiration){
		if (!$customCacheExpiration){
			$this->cache->save($query, $data);
		} elseif (is_int($customCacheExpiration) && $customCacheExpiration > 0) {
			$this->cache->save($query, $data, $customCacheExpiration);
		}
	}

	function loggerSqlFormat($table, $fields) {

	}
}
