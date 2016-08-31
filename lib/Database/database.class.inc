<?php

use Ramsey\Uuid\Uuid;
use PP\Lib\Database\DatabaseInterface;
use PP\Lib\Database\DatabaseException;

/**
 * Класс работы с БД
 */
require_once PPLIBPATH . 'Database/sqladapter.class.inc';

/**Вернуть данные и таблицы в виде массива*/
define('DB_SELECT_TABLE', 1);

/**Вернуть количество строк COUNT()*/
define('DB_SELECT_COUNT', 2);

/**Вернуть данные и таблицы в виде дерева, исключая orphaned ветки */
define('DB_SELECT_TREE',  4);

/**Вернуть данные и таблицы в виде LIST*/
define('DB_SELECT_LIST',  8);

/**Вернуть данные и таблицы в виде дерева, включая orphaned ветки в корневой раздел*/
define('DB_SELECT_FULL_TREE',  16);

define('DB_SELECT_WITHOUT_EXTERNALS',  32);

/**Значение не NULL*/
define('NOT_NULL', '~~___IS_NOT_NULL___~~');

/**
 * Class PXDatabase
 *
 * @TODO: make sys_* fields as constants for this class
 *
 * @method string GetError() Get driver last error
 * @method string EscapeString($unescaped) Escape string via concrete driver implementation
 * @method mixed MapData($value) Escape mixed variable passed
 * @method mixed mapFields($field) Escape string variable
 * @method int|null ModifyingQuery($query, $table = null, $retField = null, $flushCache = true, $retCount = false)
 * @method int|null|resource|void UpdateObjectById($table, $objectId, $fields, $values, $flushCache = true)
 * @method void transactionBegin()
 * @method void transactionCommit()
 * @method void transactionRollback()
 */
class PXDatabase extends NLSQLDatabaseAdapter implements DatabaseInterface {

	/** @var PXApplication */
	public $app;

	/** @var array */
	public $types;

	/** @var PXTriggerDescription[] */
	public $triggers;

	/** @var PXUser|PXUserAuthorized */
	public $user;

	/**
	 * new Behavior added
	 * If this flag set to true, database will throws exception , instead of error triggering.
	 * @var bool
	 */
	public $throwException = false;

	/**
	 * Конструктор класса
	 *
	 * @param PXApplication $app {@link $app} reference to PXApplication
	 * @param string $dbDescriptionName
	 */
	function __construct($app, $dbDescriptionName = 'database') {
		parent::__construct($app->dbDescription[$dbDescriptionName]);

		$this->setUser(new PXUserNull());

		$this->app      = $app;
		$this->types    = $app->types;
		$this->triggers = $this->app->triggers->{$dbDescriptionName};
	}

	protected function FatalError($message='') {
		if($this->throwException){
			throw new DatabaseException($message);
		}
		FatalError($message);
	}


	function setUser($user) {
		$this->user = $user;
	}

	/**
	 * Функуция перегружает функцию родительского класса.
	 * Родительский query возвращает массив в случае успеха и отрицательное целое в случае неуспеха.
	 * Эта функция обрабатывает отрицательное целое и всегда возвращает массив
	 *
	 * @param string $query SQL-query
	 * @param bool $donotusecache
	 * @param array $limitpair
	 * @return array
	 */
	function query($query, $donotusecache = false, $limitpair = NULL) {
		$resTable = parent::query($query, $donotusecache, $limitpair);

		if ($resTable == ERROR_DB_CANNOTCONNECT) {
			$this->FatalError('Невозможно подсоединиться к БД');

		} elseif ($resTable == ERROR_DB_BADQUERY) {
			$this->FatalError($this->getError());
		}

		return $resTable;
	}

	/**
	 * Загружает каталоги (directories) помеченные в datatypes.xml как schema="sql" && load="automatic"
	 * @see LoadSQLDirectory
	 *
	 * @param array $directories reference to PXDirectoryDescription objects array
	 */
	function LoadDirectoriesAutomatic(&$directories) {
		foreach ($directories as $k=>$v) {
			if ($v->schema != 'sql')       continue;
			if ($v->load   != 'automatic') continue;
			$this->LoadSQLDirectory($directories[$k]);
		}
	}

	/**
	 * Загружает справочники (directories)
	 *
	 * @uses LoadDirectory
	 *
	 * @todo docme подробнее
	 *
	 * @param mixed $format
	 */
	function LoadDirectoriesByType(&$format, $object=NULL) {
		foreach ($format->fields as $k=>$v) {
			if (
				is_object($v->values) &&
				strtolower(get_class($v->values)) == 'pxdirectorydescription' &&
				!$v->values->loaded
			) {
				$this->LoadDirectory($format->fields[$k]->values, $object, true, $format);
			}
		}
	}

	/**
	 * Загружает справочник (directories) в зависимости от типа
	 *
	 * @uses LoadSQLDirectory, PXApplication::LoadXMLDirectory
	 *
	 * @param PXDirectoryDescription $directory
	 */
	function LoadDirectory(PXDirectoryDescription $directory, $object, $addDefault = true, $format = NULL){
		switch($directory->schema) {
			case 'sql':
				$this->LoadSQLDirectory($directory, $addDefault, $object, $format);
				break;

			case 'xml':
				$this->app->LoadXMLDirectory($directory, $addDefault);
				break;
		}
	}


	function loadDropdownValuesForListedFields($format, $objects) {
		foreach($format->fields as $k => $field) {
			if($field->listed && strlen($field->source)) {
				$directory = $this->app->directory[$field->source];

				$idsArray = getColFromTable($objects, $field->name);
				if (empty($idsArray)) {
					// nothing to update
					continue;
				}

				$flattenArray = array();
				foreach ($idsArray as $value) {
					// $idsArray is rarely not flattened, so cast to array is slower than check
					if (is_array($value)) {
						foreach ($value as $v) {
							$flattenArray[$v] = null;
						}
					} else {
						$flattenArray[$value] = null;
					}
				}
				$flattenArray = array_keys($flattenArray);

				$needUpdate = (count($directory->filter) != count($flattenArray));
				if (!$needUpdate) {
					$needUpdate = count(array_diff($flattenArray, $directory->filter));
				}
				if ($needUpdate) {
					$directory->filter = $flattenArray;
					$this->LoadDirectory($directory, array(),true, $format);
				}
			}
		}
	}


	/**
	 * Парсит WHERE в справочниках
	 *
	 * @uses MapData
	 * @uses _NormalizeObjectAttribute
	 *
	 * @todo docme подробнее
	 *
	 * @param mixed $where
	 * @param mixed $object
	 * @param mixed $format
	 * @return string
	 */
	function ParseWhereTemplate($where, $object, $format) {
		if(strpos($where, 'THIS.') !== false) {
			if(is_array($object)) {
				preg_match_all('/THIS\.([A-Z0-9\-_]+)/'.REGEX_MOD, $where, $backhref);

				foreach($backhref[1] as $b) {
					if(array_key_exists(mb_strtolower($b), $object)) {
						if(is_null($format)) {
							$replace = $this->MapData($object[strtolower($b)]);
						} else {
							$nullObj = null;
							$replace = $this->MapData($this->_NormalizeObjectAttribute(strtolower($b), $object, $format->fields[strtolower($b)], $nullObj, $nullObj));
						}

						$where = str_replace('THIS.'.$b, $replace, $where);
					} else {
						$where = 'FALSE';
					}
				}

			} else {
				$where = 'FALSE';
			}
		}

		if(strpos($where, 'PROPERTY.') !== false) {
			preg_match_all('/PROPERTY\.([A-Z0-9_]+)/'.REGEX_MOD, $where, $backhref);
			foreach($backhref[1] as $b) {
				$prop = $this->app->GetProperty($b);

				if(!is_null($prop)) {
					$where = str_replace('PROPERTY.'.$b, $this->MapData($prop), $where);
				} else {
					$where = 'FALSE';
				}
			}
		}

		return $where;
	}

	/**
	 * Загружает по ссылке справочник (directory)  помеченный в datatypes.xml как schema="sql"
	 * Использует {@link PXDatabase::_GetData()} для загрузки каталогов из базы данных
	 *
	 * @param PXDirectoryDescription $directory
	 */
	function LoadSQLDirectory(&$directory, $addDefault = true, $object=NULL, $format=NULL) {
		$location = $directory->location;

		$location = $this->ParseWhereTemplate($location, $object, $format);

		$directory->filter = array_filter($directory->filter);
		if(sizeof($directory->filter)) {
			$tmpWhere = array();
			if(strlen($location)) {
				$tmpWhere[] = $location;
			}
			$tmpWhere[] = $directory->source.'.id IN ('.implode(',', $directory->filter).')';

			$location = implode(' AND ', $tmpWhere);
		}

		$format = clone $this->types[$directory->source];

		/*
		 * fixme: check this code please
		 * looks like it's totally wrong!
		 */

		foreach($format->fields as $field) {
			if (in_array($field->name,array('pid','parent'))) {
				$directory->parentField = !isset($format->fields['pid']) ? 'parent' : 'pid';
			}

			if(!$field->listed && !in_array($field->name,array('id', $directory->parentField))) {
				unset($format->fields[$field->name]);
			}
		}

		$directory->values = $this->_GetData($format, null, DB_SELECT_TABLE, $location);
		$directory->SetLoaded($addDefault);
	}

	/**
	 * Конструирует и исполняет запрос в БД, возвращает массив с данными
	 * Использует {@link NLSQLDatabaseAdapter::Query()} для запросов
	 *
	 * @param PXTypeDescription $format
	 * @param int $mode request mode. Default = {@link DB_SELECT_TABLE}
	 * @param string $where raw SQL WHERE clause
	 * @param array|null $limit null or array(LIMIT,OFFSET)
	 * @return array|null results of {@link NLSQLDatabaseAdapter::Query()}
	 */
	protected function _GetData($format, $status, $mode, $where=NULL, $limit=NULL, $order=NULL, $dontUseCache = false) {
		if(is_array($limit)) {
			if(is_null($limit[0]) || is_null($limit[1])) {
				$limit = null;
			} else {
				$limit = array_map('intval', $limit);
			}
		}

		PXProfiler::begin('GET DATA: ' . $format->id . '[' . (is_null($status)?'N':($status?'t':'f')) . ', "' . $where . '"]-[' .  (is_array($limit) ? implode('.', $limit) : '') . ']-[' . $order . ']');
		$where = $this->joinWhere($format, $status, $where);

		$order_by = null;

		if ($mode == DB_SELECT_COUNT) {
			$selector = 'count(' . $format->id . '.id)';
		} else {
			$selector     = $this->_createSelector($format);
			$order        = is_null($order) ? $format->order : $order;
			$first_is_fun = preg_match('#^\s*\w+\s*\(#S'.REGEX_MOD, $order);
			$order_by     = ($first_is_fun ? '' : $format->id . '.') . $order;
		}

		foreach ($this->triggers as $t) {
			$t->getTrigger()->OnBeforeGetDataBuildQuery($this, $format, $mode, $selector, $where, $order_by, $limit);
		}

		$query = 'SELECT ' . $selector . ' FROM ' . $format->id;

		if (!is_null($where))    $query .= ' WHERE ' . $where;
		if (!is_null($order_by)) $query .= ' ORDER BY ' . $order_by;

		foreach ($this->triggers as $t) {
			$t->getTrigger()->OnGetDataBuildQuery($this, $format, $mode, $query, $selector, $where, $order_by, $limit);
		}

		$queryIden = $this->_getCacheIden($query, $mode, $limit);
		$data = $this->cache->load(array($queryIden, $format->id));

		if ($data === null || $dontUseCache) {
			$sqlRes = $this->Query($query, true, $limit);
			$data = $this->_PrepareResult($sqlRes, $format, $mode);
			$this->cache->save(array($queryIden, $format->id), $data);
		}

		PXProfiler::end();
		return $data;
	}

	private function joinWhere($format, $status, $where=null) {
		$w = array();
		if(!is_null($status)) {
			$w[] = $format->id.'.status='.$this->trueStatusString($status);
		}

		if(!is_null($where)) {
			switch(true) {
				case is_string($where):
					$w[] = $where;
					break;

				case is_array($where):
					$w = array_merge($w, $where);
					break;
			}
		}

		if(!sizeof($w)) {
			return null;
		}

		return  implode(' AND ', $w);
	}

	protected function _getCacheIden($query, $mode, $limit) {
		return sprintf('%s;mode:%s;limit:%s;groups:%s;acl:',
			$query,
			$mode,
			is_array($limit) ? join('.', $limit) : '',
			isset($this->user->groups) ? join('.', $this->user->groups) : '',
			$this->user->aclType()
		);
	}


	/**
	 * @param PXTypeDescription $format
	 * @return string
	 */
	protected function _createSelector($format) {
		$hasSysMeta = false;
		$formatFields = [];

		/**
		 * @var string $k field name
		 * @var PXFieldDescription $v
		 */
		foreach ($format->fields as $k => $v) {
			if (!$v->storageType->storedInDb()) {
				$hasSysMeta = $hasSysMeta || $v->storageType->storedInMeta();
				continue;
			}

			$p = [];
			$formatFields[] = $v->storageType->createSelector($v, null, $p);
		}

		if (count($format->childs)) {
			$formatFields[] = $format->id.'.'.OBJ_FIELD_CHILDREN;
		}

		$formatFields[] = $format->id.'.'.OBJ_FIELD_CREATED;
		$formatFields[] = $format->id.'.'.OBJ_FIELD_MODIFIED;
		$formatFields[] = $format->id.'.'.OBJ_FIELD_ORDER;
		$formatFields[] = $format->id.'.'.OBJ_FIELD_UUID;
		if ($format->id != DT_USER) {
			$formatFields[] = $format->id.'.'.OBJ_FIELD_OWNER;
		}

		if ($hasSysMeta) {
			$formatFields[] = $format->id.'.'.OBJ_FIELD_META;
		}

		return implode(', ', $formatFields);
	}

	/**
	 * Возвращает массив данных из БД для заданного типа объектов {@link PXTypeDescription}.
	 * Использует {@link PXDatabase::_GetData()} для запросов.
	 *
	 * @param PXTypeDescription $format
	 * @param int $id
	 * @return array
	 */
	function getObjectById($format, $id, $dontUseCache = false) {
		if (empty($id)) {
			return [];
		}

		if (!is_numeric($id)) {
			$this->FatalError('Попытка SQL-Injection!');
		}

		$data = $this->_GetData(
			$format,
			null,
			DB_SELECT_TABLE,
			$format->id . '.id = ' . $id,
			null,
			null,
			$dontUseCache
		);

		if (count($data)) {
			return reset($data);
		} else {
			return [];
		}
	}

	/**
	 * Возвращает объекты из БД по массиву числовых значений для поля (Integer)
	 * Использует {@link NLSQLDatabaseAdapter::Query()} для запросов
	 *
	 * @param PXTypeDescription $format
	 * @param string $status objects status ('TRUE' | 'FALSE')
	 * @param string $field integer field in DB for search
	 * @param array  $intArray array of int values
	 * @param int $mode request mode. Default = {@link DB_SELECT_TABLE}
	 * @return array
	 */
	function getObjectsByIntArray($format, $status, $field, array $intArray, $mode=DB_SELECT_TABLE, $order=NULL) {
		return $this->getObjectsByIntArrayLimited($format, $status, $field, $intArray, null, null, $mode, $order);
	}
	function getObjectsByIntArrayLimited($format, $status, $field, array $intArray, $limit, $offset, $mode=DB_SELECT_TABLE, $order=NULL) {
		if (!isset($format->fields[$field])) {
			$this->FatalError(sprintf('В модели "%s" отсутствует поле "%s"!', $format->id, $field));
		}

		foreach ($intArray as $val) {
			if (!is_numeric($val)) {
				$this->FatalError('Попытка SQL-Injection!');
			}
		}

		if (count($intArray)) {
			$whereClause = sprintf('%s.%s IN (%s)', $format->id, $field, implode(',', $intArray));
			$res = $this->_GetData($format, $status, $mode, $whereClause, array($limit, $offset), $order);
		} else {
			$res = $this->emptyResult($mode);
		}

		return $res;
	}

	/**
	 * Возвращает объекты из БД по массиву id (Integer)
	 * Использует {@link NLSQLDatabaseAdapter::Query()} для запросов
	 *
	 * @param PXTypeDescription     $format
	 * @param String $status        objects status ('TRUE' | 'FALSE')
	 * @param Array  $idArray       array of objects id
	 * @param Integer $mode         request mode. Default = {@link DB_SELECT_TABLE}
	 * @return Array
	 */
	function getObjectsByIdArray($format, $status, $idArray, $mode=DB_SELECT_TABLE, $order=NULL) {
		return $this->getObjectsByIntArray($format, $status, 'id', $idArray, $mode, $order);
	}

	/**
	 * Возвращает массив данных из БД для заданного типа объектов {@link PXTypeDescription}.
	 * Использует {@link PXDatabase::_GetData()} для запросов
	 *
	 * @param PXTypeDescription $format
	 * @param String $status            objects status ('TRUE' | 'FALSE')
	 * @param Integer $mode             request mode. Default = {@link DB_SELECT_TABLE}
	 * @return Array
	 */
	function getObjects($format, $status, $mode=DB_SELECT_TABLE, $order=NULL) {
		return $this->getObjectsLimited($format, $status, null, null, $mode, $order);
	}
	function getObjectsLimited($format, $status, $limit, $offset, $mode=DB_SELECT_TABLE, $order=NULL) {
		return $this->_GetData($format, $status, $mode, null, array($limit, $offset), $order);
	}

	/**
	 * Возвращает массив из БД для заданного типа объектов {@link PXTypeDescription}.
	 * Использует {@link PXDatabase::_GetData()} для запросов.
	 *
	 * @param PXTypeDescription $format
	 * @param String $status    objects status ('TRUE' | 'FALSE')
	 * @param Mixed $parent     parent id value (numeric, array or NULL)
	 * @param Integer $mode     request mode, default = {@link DB_SELECT_TABLE}
	 * @return Array
	 */
	function getObjectsByParent($format, $status, $parent, $mode=DB_SELECT_TABLE, $order=NULL) {
		return $this->getObjectsByParentLimited($format, $status, $parent, null, null, $mode, $order);
	}
	function getObjectsByParentLimited($format, $status, $parent, $limit, $offset, $mode=DB_SELECT_TABLE, $order=NULL) {
		if (is_numeric($parent)) {
			$whereClause = $format->id.'.parent = '.$parent;

		} elseif (is_null($parent)) {
			$whereClause = $format->id.'.parent IS NULL';

		} elseif (is_array($parent)) {
			return $this->getObjectsByIntArrayLimited($format, $status, 'parent', $parent, $limit, $offset, $mode, $order);
		} else {
			$this->FatalError('Попытка SQL-Injection!');
		}

		return $this->_GetData($format, $status, $mode, $whereClause, array($limit, $offset), $order);
	}

	/**
	 * Возвращает массив из БД для заданного типа объектов {@link PXTypeDescription}.
	 * Использует {@link PXDatabase::_GetData()} для запросов.
	 *
	 * @param PXTypeDescription $format
	 * @param String $status    objects status ('TRUE' | 'FALSE')
	 * @param String $where     sql where
	 * @param Integer $mode     request mode, default = {@link DB_SELECT_TABLE}
	 * @return Array
	 */
	function getObjectsByWhere($format, $status, $whereClause, $mode=DB_SELECT_TABLE, $order=NULL) {
		return $this->getObjectsByWhereLimited($format, $status, $whereClause, null, null, $mode, $order);
	}
	function getObjectsByWhereLimited($format, $status, $whereClause, $limit, $offset, $mode=DB_SELECT_TABLE, $order=NULL) {
		if(is_array($whereClause)) {
			$whereClause = implode(' AND ', $whereClause);
		}
		return $this->_GetData($format, $status, $mode, $whereClause, array($limit, $offset), $order);
	}


	/**
	 * Возвращает массив из БД для заданного типа объектов {@link PXTypeDescription}.
	 * Использует {@link PXDatabase::_GetData()} для запросов.
	 *
	 * @param PXTypeDescription $format
	 * @param String $status    objects status ('TRUE' | 'FALSE')
	 * @param String $field     field name
	 * @param String $value     field value
	 * @param Integer $mode     request mode, default = {@link DB_SELECT_TABLE}
	 * @return Array
	 */
	function getObjectsByFields($format, $status, $fields, $mode=DB_SELECT_TABLE, $order=NULL) {
		return $this->getObjectsByField($format, $status, $fields, null, $mode, $order);
	}

	function getObjectsByFieldsLimited($format, $status, $fields, $limit, $offset, $mode=DB_SELECT_TABLE, $order=NULL) {
		return $this->getObjectsByFieldLimited($format, $status, $fields, null, $limit, $offset, $mode, $order);
	}
	function getObjectsByField($format, $status, $field, $value, $mode=DB_SELECT_TABLE, $order=NULL) {
		return $this->getObjectsByFieldLimited($format, $status, $field, $value, null, null, $mode, $order);
	}
	function getObjectsByFieldLimited($format, $status, $field, $value, $limit, $offset, $mode=DB_SELECT_TABLE, $order=NULL) {
		if (! is_array($field)) {
			$field = array($field => $value);
		}

		$whereClause = array();
		foreach ($field as $k => $v) {
			$not = false;
			if (strpos($k, '!') === 0) {
				$not = true;
				$k = str_replace('!', '', $k);
			}
			//заменяет спец.значения на соотв. sql-выражения
			switch (true) {
				case is_null($v):
					$val = sprintf(' IS %sNULL', $not ? "NOT " : "");
					break;

				case $v === NOT_NULL:
					$val = ' IS NOT NULL';
					break;

				case is_array($v):
					empty($v) && $this->FatalError("Array value for db#getObjectsByField* can't be empty!");
					$db = $this;
					$val = ($not ? " NOT" : "") . " IN ('" . join("','", array_map(function($item) use ($db) { return $db->EscapeString($item); }, $v)) . "')";
					break;

				case is_bool($v):
					$val = ($not ? ' <> ' : ' = ') . $this->trueStatusString($v);
					break;

				default:
					$val = ($not ? ' <> ' : ' = ') . "'".$this->EscapeString($v)."'";
			}
			$whereClause[] = $format->id.'.'.$k.$val;
		}
		$whereClause = implode($whereClause, ' AND ');

		return $this->_GetData($format, $status, $mode, $whereClause, array($limit, $offset), $order);
	}

	/** @see getObjectsByField */
	function getObjectsByFieldLike($format, $status, $field, $value, $mode=DB_SELECT_TABLE, $order=NULL) {
		return $this->getObjectsByFieldLikeLimited($format, $status, $field, $value, null, null, $mode, $order);
	}
	function getObjectsByFieldLikeLimited($format, $status, $field, $value, $limit, $offset, $mode=DB_SELECT_TABLE, $order=NULL) {
		$field = $this->EscapeString($field);
		$whereClause  = $format->id.'.'.$field.$this->LIKE($value);

		return $this->_GetData($format, $status, $mode, $whereClause, array($limit, $offset), $order);
	}

	/** @see getObjectsByField */
	function getObjectsByFieldNotEmpty($format, $status, $field, $mode=DB_SELECT_TABLE, $order=NULL) {
		return $this->getObjectsByFieldNotEmptyLimited($format, $status, $field, null, null, $mode=DB_SELECT_TABLE, $order=NULL);
	}
	function getObjectsByFieldNotEmptyLimited($format, $status, $field, $limit, $offset, $mode=DB_SELECT_TABLE, $order=NULL) {
		$field = $this->EscapeString($field);
		$whereClause = 'char_length('.$format->id.'.'.$field.') > 0';
		return $this->_GetData($format, $status, $mode, $whereClause, array($limit, $offset), $order);
	}

	/**
	 * Возвращает массив данных из БД для заданного типа объектов {@link PXTypeDescription}.
	 * Производит поиск $world по ВСЕМ полям указанным в $format.
	 * Использует {@link PXDatabase::_GetData()} для запросов.
	 *
	 * @param PXTypeDescription $format
	 * @param String $status    objects status ('TRUE' | 'FALSE')
	 * @param String $word      search word
	 * @param Integer $mode     request mode, default = {@link DB_SELECT_TABLE}
	 * @return Array
	 */
	function getObjectsBySearchWord($format, $status, $word, $mode=DB_SELECT_TABLE, $order=NULL) {
		return $this->getObjectsBySearchWordLimited($format, $status, $word, null, null, $mode, $order);
	}
	function getObjectsBySearchWordLimited($format, $status, $word, $limit, $offset, $mode=DB_SELECT_TABLE, $order=NULL) {
		$word = $this->EscapeString($word);
		$whereClause = '';
		foreach ($format->fields as $k=>$v) {
			$p = array('formatId'=>$format->id, 'word'=>$word);
			$whereClause .= $v->storageType->searchLikeQuery($v, array(), $p);
		}
		$whereClause = mb_substr($whereClause, 0, -4);
		if(!strlen($whereClause)) {
			return $this->emptyResult($mode);
		}

		return $this->_GetData($format, $status, $mode, $whereClause, array($limit, $offset), $order);
	}

	protected function emptyResult($mode) {
		switch($mode) {
			case DB_SELECT_TABLE:
				$res = array();
				break;
			case DB_SELECT_COUNT:
				$res = 0;
				break;
			default:
				$res = null;
		}
		return $res;
	}

	/**
	 * Добавляет объект в БД.
	 * Перед добавлением объект преобразуется к необходимому storage type.
	 * Перед добавлением объекта вызываются триггеры. Если хотя бы один OnBeforeAddObject
	 * триггер вернёт false, добавление не произойдёт.
	 * После успешного добавления объекта вызываются триггеры OnAddObject.
	 * Метод использует {@link PXDatabase::InsertObject} для добавления объектов.
	 *
	 * @see InsertObject
	 * @see ModifyingQuery
	 *
	 * @param PXTypeDescription $format
	 * @param array $object array of assoaciative arrays 'db_field_name'=>'value'
	 * @return int
	 */
	function AddContentObject($format, $object) {
		$audit = PXAuditLogger::getLogger();

		if (!$this->user->can(array('write', 'add'), $format, $object)) {
			$errString = 'Отказано в доступе при попытке создать объект типа <em>'.$format->id.'</em>';
			$audit->error($errString, $format->id .'/0');
			$this->FatalError($errString);
		}

		$this->checkForParent($format, $object);

		$uuidV4 = Uuid::uuid4();
		$dbFields = [];
		$dbValues = [];
		foreach ($format->fields as $k => $v) {
			if ($k === 'pathname') {
				$nullObject = null;
				$object[$k] = $this->_NormalizeObjectPathName($format, $object, $nullObject);
			}

			$this->_NormalizeObjectAttribute($k, $object, $v, $dbFields, $dbValues);
		}

		// sys fields
		$dbFields[] = OBJ_FIELD_CREATED;
		$dbFields[] = OBJ_FIELD_MODIFIED;
		$dbFields[] = OBJ_FIELD_UUID;
		$dbValues[] = '##now##';
		$dbValues[] = '##now##';
		$dbValues[] = $uuidV4;

		if ($format->id != DT_USER) {
			$dbFields[] = OBJ_FIELD_OWNER;
			$dbValues[] = $this->user->id;
		}

		if (count($format->childs)) {
			$dbFields[] = OBJ_FIELD_CHILDREN;
			$_children = array();
			foreach ($format->childs as $child) {
				if (!is_null($this->types[$child]->byDefault)) {
					$_children[$child] = $this->types[$child]->byDefault;
				}
			}

			// TODO: replace with json_encode
			$dbValues[] = serialize($_children);
		}

		$permitAdd = true;
		$newId = 0;
		foreach ($this->triggers as $t){
			$permitAdd = $t->getTrigger()->OnBeforeAddObject($this, $newId, $format, $object, $dbFields, $dbValues);

			if(!$permitAdd) {
				break;
			}
		}

		if ($permitAdd) {
			// формируем список полей которые не будут сохранены в базу данных
			// ориентируемся на результат выполнения storedInMeta, для файловых
			// и виртуальных полей метод должен возвращать true.
			$cuttedFieldDiff = array_diff(array_keys($format->fields), $dbFields);
			$sysMetaFieldList = array();
			foreach($cuttedFieldDiff as $k) {
				if($format->fields[$k]->storageType->storedInMeta()) {
					$sysMetaFieldList[] = $k;
				}
			}

			// открываем транзакцию
			$this->transactionBegin();
			$newId = $this->InsertObject($format->id, $dbFields, $dbValues, false);
			if ($newId < 0) {
				$errString = $this->GetError();
				$audit->error($errString, $format->id .'/'.getFromArray($object, 'parent'));
				$this->transactionRollback();
				$this->FatalError($errString);
			}

			$object[OBJ_FIELD_ID] = $newId;

			// обходим все поля, в попытке найти что-то похожее на файл
			$sysMetaField = array();
			foreach ($format->fields as $k=>$v) {
				$param = [
					OBJ_FIELD_ID => $newId,
					'format' => $format->id,
					OBJ_FIELD_META => [],
				];

				$proceedFileResult = $v->storageType->proceedFile($v, $object, $param);
				if(in_array($k, $sysMetaFieldList) && $proceedFileResult) {
					$sysMetaField[$k] = $proceedFileResult;
				}
			}

			// обновляем sys_meta, если есть что туда записать.
			$sqlUpdateRule = [];
			$sqlUpdateRule[] = OBJ_FIELD_ORDER."=".OBJ_FIELD_ID;

			if (count($sysMetaField) > 0) {
				$sqlUpdateRule[] = OBJ_FIELD_META."=".$this->MapData(json_encode($sysMetaField));
			}

			$this->ModifyingQuery(
				sprintf(
					'UPDATE %s SET %s WHERE id = %s',
					$format->id,
					implode(', ', $sqlUpdateRule),
					$newId
				),
				null,
				null,
				false
			);

			$this->clearObjectTypeCache($format->id);
			$this->transactionCommit();

			$object[OBJ_FIELD_ORDER] = $newId;
			$object[OBJ_FIELD_META] = $sysMetaField;
			$object[OBJ_FIELD_UUID] = $uuidV4;

			foreach ($this->triggers as $t) {
				$t->getTrigger()->OnAddObject($this, $newId, $format, $object, $dbFields, $dbValues);
			}

			$audit->info('Объект добавлен', $format->id .'/'. $newId);
		}

		return $newId;
	}

	/**
	 * Добавляет объекты в БД.
	 *
	 * Перед добавлением преобразуются к необходимому storage type.
	 * После добавления объектов триггеры НЕ вызываются.
	 * Использует {@link PXDatabase::InsertObject} для добавления объектов.
	 *
	 * @see InsertObject
	 * @see ModifyingQuery
	 *
	 * @param PXTypeDescription $format
	 * @param Array $objectsArray   array of assoaciative arrays 'db_field_name'=>'value'
	 */
	function AddContentObjects($format, $objectsArray) {
		$audit = PXAuditLogger::getLogger($this->app, $this);

		if (count($objectsArray) == 0) {
			return;
		}

		PXProfiler::begin('Add '.sizeof($objectsArray).' '.$format->id.' objects');

		if (isset($format->fields['parent'])) {
			$parentIds = array_unique(GetColFromTable($objectsArray, 'parent'));

			foreach ($parentIds as $parentId) {
				$objectInDB = $this->getObjectById($this->types[$format->parent], $parentId);

				if (!count($objectInDB)) {
					$this->FatalError('Попытка добавить объект в несуществующий раздел');
				}

				if ($format->id != $format->parent && !in_array($format->id, $this->app->GetAllowedChildsKeys($format->parent, $objectInDB))) {
					$this->FatalError('Объект данного типа не может быть добавлен в данный раздел');
				}

				if (!$this->user->can(array('write', 'add'), $format, array('id' => 0, 'parent' => $parentId))) {
					$errString = 'Отказано в доступе при попытке создать объект типа <em>'.$format->id.'</em>';
					$audit->error($errString, $format->id .'/0');
					$this->FatalError($errString);
				}
			}
		}

		$copyCols  = array();
		$copyTable = array();
		$idx       = 0;
		$lastOrder = current(current($this->Query('SELECT max(sys_order) FROM '.$format->id, TRUE)));

		$pathnames = array();
		$nullObj = null;

		PXProfiler::begin('normaize/prepare foreach '.sizeof($format->fields).'x'.sizeof($objectsArray));

		foreach ($objectsArray as $object) {
			$copyTable[$idx] = array();

			foreach ($format->fields as $k=>$v) {
				if ($k === 'pathname') {
					$object[$k] = $this->_NormalizeObjectPathName($format, $object, $pathnames);
				}

				if ($idx == 0) {
					$this->_NormalizeObjectAttribute($k, $object, $v, $copyCols, $copyTable[$idx]);
				} else {
					$this->_NormalizeObjectAttribute($k, $object, $v, $nullObj, $copyTable[$idx]);
				}
			}

			if ($idx == 0) {
				$copyCols[] = OBJ_FIELD_ORDER;
			}

			$copyTable[$idx][] = ++$lastOrder;

			// sys fields
			if ($idx == 0) {
				$copyCols[] = OBJ_FIELD_CREATED;
				$copyCols[] = OBJ_FIELD_MODIFIED;
			}

			$copyTable[$idx][] = '##now##';
			$copyTable[$idx][] = '##now##';

			if ($format->id != DT_USER) {
				if($idx == 0) {
					$copyCols[] = OBJ_FIELD_OWNER;
				}

				$copyTable[$idx][] = $this->user->id;
			}

			if (count($format->childs)) {
				if ($idx == 0) {
					$copyCols[] = OBJ_FIELD_CHILDREN;
				}

				$_children = array();
				foreach ($format->childs as $child) {
					if (!is_null($this->types[$child]->byDefault)) {
						$_children[$child] = $this->types[$child]->byDefault;
					}
				}

				// TODO: replace with json_encode
				$copyTable[$idx][] = serialize($_children);
			}

			$idx++;
		}
		PXProfiler::end();

		$res = $this->ModifyingCopy($format->id, $copyCols, $copyTable);
		$this->clearObjectTypeCache($format->id);
		$audit->info('Добавлено '.$idx.' объектов', $format->id);

		PXProfiler::end();

		return $res;
		// TRIGGER ?
	}


	function cloneContentObject(&$format, $objectData, $status=false) {
		$audit = PXAuditLogger::getLogger($this->app, $this);

		if(is_array($objectData) && isset($objectData['id']) && is_numeric($objectData['id']) && $objectData['id'] > 0) {
			$donorObject = $objectData;
		} else {
			$donorObject = $this->getObjectById($format, $objectData);
		}

		$object           = $donorObject;
		$object['id']     = null;
		$object['status'] = $status;

		$permitClone = true;
		foreach ($this->triggers as $t){
			$permitClone = $t->getTrigger()->onBeforeCloneObject($this, $format, $object, $donorObject);
			if(!$permitClone) break;
		}

		if (!$permitClone) {
			return false;
		}

		$object['id'] = $this->addContentObject($format, $object);

		foreach ($format->fields as $k=>$v) {
			$p = array('donor'=>$donorObject, 'format'=>$format->id);
			$v->storageType->cloneFile($v, $object, $p);
		}

		foreach ($this->triggers as $t){
			$t->getTrigger()->onCloneObject($this, $format, $object, $donorObject);
		}

		$audit->info('Объект клонирован', $format->id .'/'. $donorObject['id']);

		return $object['id'];
	}

	/**
	 * Изменяет хранящийся в БД объект
	 * Работает по аналогии с {@link PXDatabase::AddContentObject()}
	 *
	 * @see AddContentObject
	 *
	 * @param PXTypeDescription $format
	 * @param array $object
	 * @param bool $skipHidden
	 * @return null
	 */
	function ModifyContentObject($format, $object, $skipHidden = false) {
		$audit = PXAuditLogger::getLogger();

		if (!$this->user->can(array('write', 'modify'), $format, $object)) {
			$errString = 'Отказано в доступе при попытке модифицировать объект типа <em>'.$format->id.'</em>';
			$audit->error($errString, $format->parent .'/'. $object['parent']);
			$this->FatalError($errString);
		}

		$objectInDB = $this->getObjectById($format, $object['id']);
		if (!count($objectInDB)) {
			$errString = 'Попытка модифицировать отсутствующий объект';
			$audit->error($errString, $format->id .'/'.$object['id']);
			$this->FatalError($errString);
		}

		if (!$this->user->can(array('write', 'modify'), $format, $objectInDB)) {
			$errString = 'Отказано в доступе при попытке модифицировать объект типа <em>'.$format->id.'</em>';
			$audit->error($errString, $format->parent .'/'. $object['parent']);
			$this->FatalError($errString);
		}

		$this->checkForParent($format, $object);

		$dbFields = [];
		$dbValues = [];

		foreach ($format->fields as $k=>$v) {
			if ($k === OBJ_FIELD_PATHNAME) {
				$nullObject = NULL;
				$object[$k] = $this->_NormalizeObjectPathName($format, $object, $nullObject);
			}

			$p = [
				'skipHidden' => $skipHidden,
				'objectInDB' => $objectInDB,
			];
			$skipThisField = $v->displayType->preModifyObject($v, $object, $p);
			if ($skipThisField) {
				continue;
			}

			//update fields, but skip not set values (keep original)
			if (array_key_exists($k, $object)) {
				$this->_NormalizeObjectAttribute($k, $object, $v, $dbFields, $dbValues);
			}
		}

		$permitModify = true;
		foreach ($this->triggers as $t) {
			$permitModify = $t->getTrigger()->OnBeforeModifyObject(
				$this,
				$object[OBJ_FIELD_ID],
				$format,
				$object,
				$dbFields,
				$dbValues,
				$objectInDB
			);

			if(!$permitModify) {
				break;
			}
		}

		if ($permitModify) {
			// формируем список полей которые не будут сохранены в базу данных
			// ориентируемся на результат выполнения storedInMeta, для файловых
			// и виртуальных полей метод должен возвращать true.
			$cutFieldDiff = array_diff(array_keys($format->fields), $dbFields);
			$sysMetaFieldList = [];

			foreach($cutFieldDiff as $k) {
				if($format->fields[$k]->storageType->storedInMeta()) {
					$sysMetaFieldList[] = $k;
				}
			}

			$dbFields[] = 'sys_modified';
			$dbValues[] = '##now##';

			$this->transactionBegin();
			$result = $this->UpdateObjectById($format->id, $object['id'], $dbFields, $dbValues, false);
			$this->clearObjectTypeCache($format->id);

			if ($result <  0) {
				$audit->error($this->GetError(), $format->id .'/'. $object['id']);
				$this->transactionRollback();
				$this->FatalError($this->GetError());
			}
			if ($format->struct == 'tree' && isset($objectInDB['parent']) && $objectInDB['parent'] != $object['parent']) {
				$childs        = $this->getObjectsByParent($format, NULL, $objectInDB['parent'], DB_SELECT_TREE);
				$objectsToMove = $childs->GetDescendants($object['id']);
				$query = 'UPDATE '.$format->id.' SET parent='.(is_null($object['parent']) ? 'NULL' : $object['parent']).' WHERE id IN ('.implode(', ', $objectsToMove).')';
				$this->ModifyingQuery($query, null, null, false);

				if(isset($format->pid)) {
					$query = 'UPDATE '.$format->id.' SET pid=NULL WHERE id='.$object['id'];
					$this->ModifyingQuery($query, null, null, false);
				}
			}

			$sysMetaField = [];

			/**
			 * @var string $k field name
			 * @var PXFieldDescription $v
			 */
			foreach ($format->fields as $k => $v) {
				//
				// Ок, поле является sys_meta полем, возможны три варианта:
				// - загружен новый файл
				// - запрос удаления одного или нескольких файлов
				// - файлы не трогали
				//
				// если файл физически присутствует на диске, proceedFile работает верно,
				// в случае отсутствия такового (например при активации плагина pp/ceph),
				// данная функция затирает содержимое sys_meta.
				//

				if (in_array($k, $sysMetaFieldList)) {
					$param = [
						OBJ_FIELD_ID => $object[OBJ_FIELD_ID],
						'format' => $format->id,
						OBJ_FIELD_META => (isset($objectInDB[OBJ_FIELD_META]))
							? $objectInDB[OBJ_FIELD_META]
							: []
					];

					$proceedFileResult = $v->storageType->proceedFile($v, $object, $param);
					if ($proceedFileResult) {
						$sysMetaField[$k] = $proceedFileResult;
					}
				}
			}

			if (count($sysMetaFieldList) > 0) {
				//
				// обновляем sys_meta, если есть что туда записать,
				// а так же, если у объекта вообще есть поля типа storedInMeta! Иначе нарушается логика!
				// см. логику выборки в методе createSelector() (проверка has_sys_meta)!
				//

				$sqlUpdateRule = sprintf(
					"sys_meta = %s",
					(count($sysMetaField) > 0)
						? $this->MapData(json_encode($sysMetaField))
						: 'NULL'
				);

				$query = sprintf('UPDATE %s SET %s WHERE id = %s', $format->id, $sqlUpdateRule, $object['id']);
				$this->ModifyingQuery($query, null, null, false);
			}

			$this->transactionCommit();
			$object[OBJ_FIELD_ORDER] = isset($objectInDB[OBJ_FIELD_ORDER]) ? $objectInDB[OBJ_FIELD_ORDER] : null;
			$object[OBJ_FIELD_META] = $sysMetaField;
			$object[OBJ_FIELD_UUID] = $objectInDB[OBJ_FIELD_UUID];

			foreach ($this->triggers as $t) {
				$t->getTrigger()->OnModifyObject(
					$this,
					$object['id'],
					$format,
					$object,
					$dbFields,
					$dbValues,
					$objectInDB
				);
			}

			$sysKeysToRemove = [
				OBJ_FIELD_CREATED,
				OBJ_FIELD_MODIFIED,
				OBJ_FIELD_META,
				OBJ_FIELD_OWNER,
				'sys_reflex_id',
				OBJ_FIELD_ORDER,
				OBJ_FIELD_UUID,
				OBJ_FIELD_CHILDREN,
			];

			$diff = array_deep_diff($this->getObjectById($format, $object['id'], true), $objectInDB, $sysKeysToRemove);
			$audit->info('Объект изменен', $format->id . '/' . $object['id'], 'AUDIT', json_encode(array_keys($diff)));
		}

		return null;
	}

	/**
	 * Изменяет системные параметры объекта хранящегося в БД ('sys_allowed', 'sys_accessmod', 'sys_accessput')
	 * Использует {@link PXDatabase::UpdateObjectByID} для доступа к объектам.
	 *
	 * @see OBJ_FIELD_CHILDREN
	 *
	 * @param PXTypeDescription $format
	 * @param Array             $object
	 */
	function ModifyObjectSysVars($format, $object) {
		$audit = PXAuditLogger::getLogger();
		$objectInDB = $this->getObjectById($format, $object['id']);

		if (!count($objectInDB)) {
			$this->FatalError('Попытка модифицировать отсутствующий объект');
		}

		if(isset($objectInDB['parent']) && !isset($object['parent'])){
			$object['parent'] = $objectInDB['parent'];
		}

		if (!$this->user->can(array('write', 'modify'), $format, $object)) {
			$errString = 'Отказано в доступе при попытке модифицировать объект типа <em>'.$format->id.'</em>';
			$audit->error($errString, $format->parent .'/'. (isset($object['parent']) ? $object['parent'] : ''));
			$this->FatalError($errString);
		}

		$dbFields = array();
		$dbValues = array();
		if (isset($object[OBJ_FIELD_CHILDREN])) {
			$dbFields[] = OBJ_FIELD_CHILDREN;
			// TODO: replace with json_encode
			$dbValues[] = serialize($object[OBJ_FIELD_CHILDREN]);
		}
		if (count($dbFields)) {
			$result = $this->UpdateObjectByID($format->id, $object['id'], $dbFields, $dbValues, false);
			if ($result <  0) {
				$this->FatalError($this->GetError());
			}
			$this->clearObjectTypeCache($format->id);

			foreach ($this->triggers as $t)
				$t->getTrigger()->OnModifyObjectSysVars($this, $object['id'], $format, $object, $dbFields, $dbValues, $objectInDB);

			$audit->info('Изменены системные параметры объекта', $format->id .'/'. $object['id']);
		}
	}

	/**
	 * Удаляет объект (и его childrens) из БД.
	 * Использует {@link PXDatabase::ModifyingQuery} для удаления.
	 * Перед и после удаления вызываются триггеры. Если хотя бы один OnBeforeDeleteObject
	 * триггер вернёт false, удаление не произойдёт.
	 *
	 *
	 * @param PXTypeDescription $format
	 * @param int $objectId object id
	 */
	function DeleteContentObject($format, $objectId) {
		$audit = PXAuditLogger::getLogger();

		$objectInDB = $this->getObjectById($format, $objectId);
		if (!count($objectInDB)) $this->FatalError('Попытка удалить отсутствующий объект');
		if (!$this->user->can(array('write', 'delete'), $format, $objectInDB)) {
			$errString = 'Отказано в доступе при попытке удалить объект типа <em>'.$format->id.'</em>';
			$audit->error($errString, $format->parent .'/'. $objectInDB['id']);
			$this->FatalError($errString);
		}

		if ($format->struct == 'tree' && isset($format->fields['pid']) && isset($this->types[$format->parent])) {
			$children        = $this->getObjectsByParent($format, NULL, $objectInDB['parent'], DB_SELECT_TREE);
			$objectsToDelete = $children->GetDescendants($objectId);
		} else {
			$objectsToDelete = array($objectId);
		}

		$permitDelete = true;
		foreach ($this->triggers as $t) {
			$permitDelete = $t->getTrigger()->OnBeforeDeleteObject($this, $objectId, $format, $objectInDB);
			if(!$permitDelete) break;
		}

		if ($permitDelete) {
			$query  = 'DELETE FROM '.$format->id.' WHERE id IN ('.implode(', ', $objectsToDelete).')';
			$this->ModifyingQuery($query, null, null, false);
			$this->clearObjectTypeCache($format->id);

			// @TODO: site/htdocs/ai in PXDatabase, seriously?!
			$d = new NLDir(BASEPATH.'/site/htdocs/ai/'.$format->id.'/'.$objectId.'/');
			$d->Delete();

			foreach ($this->triggers as $t) {
				$t->getTrigger()->OnDeleteObject($this, $objectId, $format, $objectInDB);
			}

			$audit->info('Объект удален', $format->id .'/'. $objectId);
		}
	}

	/**
	 * Поднимает объект в списке на одну позицию
	 * Использует {@link PXDatabase::_moveContentObject}
	 *
	 * @see sys_order
	 *
	 * @param PXTypeDescription $format
	 * @param Array             $objectId   object id
	 */
	function UpContentObject($format, $objectId) {
		$this->_moveContentObject($format, $objectId, 'up');
	}

	/**
	 * Опускает объект в списке на одну позицию
	 * Использует {@link PXDatabase::_moveContentObject}
	 *
	 * @see sys_order
	 *
	 * @param PXTypeDescription $format
	 * @param Array             $objectId   object id
	 */
	function DownContentObject($format, $objectId) {
		$this->_moveContentObject($format, $objectId, 'down');
	}

	/**
	 * Изменяет позицию объект в списке нужное количество раз
	 * Использует {@link PXDatabase::_moveContentObject}
	 *
	 * @see sys_order
	 *
	 * @param PXTypeDescription $format
	 * @param Array             $objectId   object id
	 * @param Integer           $shift
	 */
	function MoveContentObject($format, $objectId, $shift=-1) {
		if(!is_numeric($shift)) {
			return;
		}
		$direction = $shift > 0 ? 'up' : 'down';
		$this->_moveContentObject($format, $objectId, $direction, abs($shift));
	}

	/**
	 * Изменяет позицию объект в списке
	 * Использует {@link PXDatabase::ModifyingQuery}
	 *
	 * @see sys_order
	 *
	 * @param PXTypeDescription $format
	 * @param Array             $objectId   object id
	 * @param String            $direction  up|down
	 */
	function _moveContentObject($format, $objectId, $direction, $steps = 1) {
		$audit = PXAuditLogger::getLogger($this->app, $this);

		$objectInDB = $this->getObjectById($format, $objectId);
		if (!count($objectInDB)) $this->FatalError('Попытка модифицировать отсутствующий объект');

		if (!$this->user->can(array('write', 'modify'), $format, $objectInDB)) {
			$errString = 'Отказано в доступе при попытке модифицировать объект типа <em>'.$format->id.'</em>';
			$audit->error($errString, $format->id .'/'. $objectInDB['id']);
			$this->FatalError($errString);
		}

		$swap = !!preg_match('#sys_order\s+DESC#i'.REGEX_MOD, $format->order);
		$down = ($swap xor ($direction == 'down'));

		$where = array();
		$where[] = sprintf('%s.sys_order %s %s', $format->id, $down ? '>' : '<', $objectInDB['sys_order']);

		if (!empty($format->parent)) {
			$parent = $objectInDB['parent'];
			$is_null = is_null($parent);
			$where[] = sprintf('%s.parent %s %s', $format->id, $is_null ? 'IS' : '=', $is_null ? 'NULL' : $parent);
		}

		if ($format->struct == 'tree' && isset($format->fields['pid'])) {
			$pid = $objectInDB['pid'];
			$is_null = is_null($pid);
			$where[] = sprintf('%s.pid %s %s', $format->id, $is_null ? 'IS' : '=', $is_null ? 'NULL' : $pid);
		}

		$where = join(' AND ', $where);
		$nearCount = $this->getObjectsByWhere($format, NULL, $where, DB_SELECT_COUNT);
		if(!$nearCount){
			return;
		}
		$offset = ($steps > $nearCount ? $nearCount : $steps) - 1;
		$nearObject = $offset >= 0 ? $this->getObjectsByWhereLimited($format, NULL, $where, 1, $offset, DB_SELECT_TABLE, 'sys_order'.($down ? '' : ' DESC')) : array();
		$permitMove = true;
		$object_id= $objectInDB['id'];

		foreach ($this->triggers as $t){
			$permitMove = $t->getTrigger()->OnBeforeMoveObject($this, $objectId, $format, $objectInDB);
			if(!$permitMove) break;
		}

		if($permitMove && sizeof($nearObject)) {
			$nearObject = current($nearObject);
			$this->transactionBegin();
				$qry = sprintf('UPDATE %s SET sys_order = sys_order + 1 WHERE sys_order %s %d', $format->id, $down ? '>' : '>=', $nearObject['sys_order']);
				$this->ModifyingQuery($qry, null, null, false);
				$qry = sprintf('UPDATE %s SET sys_order = %d WHERE %1$s.id = %d', $format->id, $nearObject['sys_order'] + ($down ? 1 : 0), $objectInDB['id']);
				$this->ModifyingQuery($qry, null, null, false);
				$this->clearObjectTypeCache($format->id);
			$this->transactionCommit();

			foreach ($this->triggers as $t) {
				$t->getTrigger()->OnMoveObject($this, $objectId, $format, $objectInDB);
			}
			$audit->info('Объект перемещен в списке', $format->id .'/'. $objectId);
		}
	}


	/**
	 * Нормализует атрибут объекта
	 *
	 * @param String            $k
	 * @param Array             $object
	 * @param String            $storageType
	 * @param Array             $dbFields
	 * @param Array             $dbValues
	 */
	function _NormalizeObjectAttribute($k, $object, $field, &$dbFields, &$dbValues) {
		$p = array(
			'dbFields' =>& $dbFields,
			'dbValues' =>& $dbValues,
			'db'       =>& $this
		);

		// очень странное поведение, но, normalizeObjectAttribute на самом деле пишет
		// новые ключи в dbFields и в dbValues, базовый метод PXStorageType не осуществляет записи в эти атрибуты.
		// у тех типов данных которые сохраняются в базе присутствует собственная имплементация метода
		// normalizeObjectAttribute
		list($v, $dbFields, $dbValues) = $field->storageType->normalizeObjectAttribute($field, $object, $p);
		return $v;
	}

	/**
	 * Нормализует PathName объекта
	 * Использует {@link PXDatabase::ModifyingQuery}
	 *
	 * @param PXTypeDescription $format
	 * @param Array             $object     object id
	 * @param Array             $pathnames
	 * @todo MOVEITASAP!
	 */
	function _NormalizeObjectPathName($format, $object, &$pathnames) {
		$url = mb_strtolower(_TranslitModern($object['pathname']));

		// part 1: make url if doesn't set
		if (!$url) {
			foreach ($format->fields as $field) {
				$p = array();
				$url = $field->storageType->createPathname($field, $object, $p);

				if($url) {
					break;
				}
			}
		}

		if (!$url) $url = mb_strtolower(_TranslitModern(mb_substr(preg_replace('/\&.+?\;/'.REGEX_MOD, '', $object['title']), 0, 32)));
		if (!$url) $url = sprintf('%08d', rand(0, 100000000));

		// part 2: triming bad chars - very readable... gj.
		$url = trim($url);
		for($i=0;$i<strlen($url);$i++) {
			$o = ord($url{$i});

			if ($o != 45 && $o != 46 && ($o < 48 || $o > 57) && ($o < 65 || $o > 90) && $o != 95 && ($o < 97 || $o > 122) && ($o != 58)) {
				$url{$i} = '_';
			}
		}

		// part 3: validating extention
		if ($format->id !== 'struct') {
			$allowed = array_flip(unserialize(ENUM_ALLOWED_PATHNAMES));
			$dotpos = strrpos($url, '.');

			if (!$dotpos || !isset($allowed[mb_substr($url, ++$dotpos)])) {
				$url .= '.html';
			}
		}

		// part 4: make unique name
		if (isset($format->parent) && isset($object['parent'])) {
			while (1) {
				if (is_array($pathnames)) {
					$reiteration = (int)isset($pathnames[$url]);
				} else {
					$exception  = isset($object['id']) ? array('id'=>$object['id']) : array();
					$condCols   = array('pathname'=>$url, 'parent'=>$object['parent']);
					$condTables = array();

					foreach ($this->types[$format->parent]->childs as $child) {
						if (isset($this->types[$child]->fields['pathname'])) {
							$condTable = array('tableName' => $child, 'exWhere' => '');

							foreach ($this->triggers as $t){
								$t->getTrigger()->OnBeforeNormalizePathBuildQuery($format, $this->types[$child], $condTable, $object);
							}

							$condTables[] = $condTable;
						}
					}

					$reiteration = (int)$this->IsUniqueColsCombination($condTables, $condCols, $exception);
				}

				// FIXME: very ugly code. No need mb_* functions here, working under _Translit'ed string
				if ($reiteration > 0) {
					if (strpos($url, '.') !== false) {
						if (is_numeric($url{strrpos($url, '.')-1})) {
							$url = substr($url, 0, strrpos($url, '.')-1).($url{strrpos($url, '.')-1} + 1).substr($url, strrpos($url, '.'));
						} else {
							$url = substr($url, 0, strrpos($url, '.')).'1'.substr($url, strrpos($url, '.'));
						}
					} else {
						if (is_numeric($url{strlen($url)-1})) {
							$url = substr($url, 0, strlen($url)-1).($url{strlen($url)-1} + 1);
						} else {
							$url = $url.'1';
						}
					}

				} else {
					$pathnames[$url] = 1;
					break;
				}
			}
		}

		return $url;
	}

	function getAllLinks($reference) {
		$query = <<<SQL
			SELECT
				{$reference->name}.*

			FROM
				{$reference->name}
SQL;

		return $this->query($query);
	}

	/**
	 *
	 * Использует {@link PXDatabase::Query}
	 *
	 * @todo docme (check params documentation)
	 *
	 * @param Mixed             $reference  Unknown
	 * @param PXTypeDescription $format
	 * @param Mixed             $id         Unknown
	 */
	 function getLinksLimited($reference, $format, $id, $limit, $offset) {
		$linkedFormat = $reference->getOther($format);

		$origType     = $this->app->types[$format];
		$linkedType   = $this->app->types[$linkedFormat];

		$query = '';

		if (is_array($id)) {
			$id = implode(',', array_map('intval', $id));
		}
		if ($linkedFormat === $format) {
			$format .= '_';
			$query = <<<SQL
				SELECT
					{$linkedFormat}.title as {$linkedFormat}title,
					{$reference->name}.*

				FROM
					{$reference->name},
					{$linkedFormat}

				WHERE
					{$reference->name}.{$format}id = {$linkedFormat}.id
						AND
					{$reference->name}.{$linkedFormat}id in ({$id})

				UNION

SQL;
		}

		$query .= <<<SQL
				SELECT
					{$linkedFormat}.title as {$linkedFormat}title,
					{$reference->name}.*

				FROM
					{$reference->name},
					{$linkedFormat}

				WHERE
					{$reference->name}.{$linkedFormat}id = {$linkedFormat}.id
						AND
					{$reference->name}.{$format}id in ({$id})
SQL;

		$limit = is_numeric($limit) && is_numeric($offset) ? array($limit, $offset) : NULL;
		$queryIden = $this->_getCacheIden($query, DB_SELECT_TABLE, $limit);
		$resTable = $this->cache->load(array($queryIden, $reference->name));

		if ($resTable === null) {
			$resTable = $this->Query($query, true, $limit);
			$this->cache->save(array($queryIden, $reference->name), $resTable);
		}

		$retTable = array();
		$param = array('db'=>$this, 'format'=>$reference->name);

		foreach ($resTable as $element) {
			$title = $element[$linkedFormat.'title'];
			$lid   = $element[$linkedFormat.'id'];
			if ($id == $lid and $linkedFormat . '_' == $format) {
				$lid = $element[$format.'id'];
			}
			unset($element[$linkedFormat.'title']);
			unset($element[$linkedFormat.'id']);
			unset($element[$format.'id']);
			$table = & $retTable[$lid];
			if(isset($retTable[$lid])){
				while(!is_null($table = & $table['next'])){
					//search first empty node in our linked list
				}
			}

			foreach($reference->fields as $fk => $fv){
				$element[$fk] = $fv->storageType->normalize($fv, $element, $param);
			}

			$table = array(
				'title'  => $title,
				'fields' => $element,
				'next'   => NULL
			);
		}
		return $retTable;
	 }

	/**
	 *
	 * Использует {@link PXDatabase::Query}
	 *
	 * @todo docme (check params documentation)
	 *
	 * @param Mixed             $reference  Unknown
	 * @param PXTypeDescription $format
	 * @param Mixed             $id         Unknown
	 */
	function GetLinks($reference, $format, $id) {
		return $this->GetLinksLimited($reference, $format, $id, NULL, NULL);
	}

	/**
	 *
	 * Использует {@link PXDatabase::Query}
	 *
	 * @todo docme (check params documentation)
	 *
	 * @param Mixed             $reference  Unknown
	 * @param PXTypeDescription $format
	 * @param Mixed             $id         Unknown
	 */
	function GetLinksCount($reference, $format, $id) {
		$linkedFormat = $reference->getOther($format);
		$query = '';


		if ($linkedFormat === $format) {
			$format .= '_';
			$query = <<<SQL
				SELECT
					COUNT({$reference->name}.*)
				FROM
					{$reference->name}
				WHERE
					{$reference->name}.{$linkedFormat}id = {$id}
				UNION

SQL;
		}


		$query .= <<<SQL
			SELECT
				count({$reference->name}.*)
			FROM
				{$reference->name}
			WHERE
				{$reference->name}.{$format}id = {$id}

SQL;

		$queryIden = $this->_getCacheIden($query, DB_SELECT_TABLE, null);
		$resTable = $this->cache->load(array($queryIden, $reference->name));

		if ($resTable === null) {
			$resTable = $this->Query($query, true);
			$this->cache->save(array($queryIden, $reference->name), $resTable);
		}

		return $resTable[0]['count'];
	}

	/*
	 * ModifyLinks wrapper
	 * */
	function addLink($fromId, $toId, $fromFormat, $toFormat) {
		$links = array(
			$toId => array(
				'on' => true,
			)
		);

		$this->modifyLinks($fromFormat->references[$toFormat->id], $fromFormat->id, $fromId, $links, false);
	}
	function replaceLink($fromId, $toId, $fromFormat, $toFormat) {
		$links = array(
			$toId => array(
				'on' => true,
			)
		);

		$this->modifyLinks($fromFormat->references[$toFormat->id], $fromFormat->id, $fromId, $links);
	}

	function deleteLink($fromId, $toId, $fromFormat, $toFormat) {
		$links = array(
			$toId => array(
				'off' => false,
			)
		);

		$this->modifyLinks($fromFormat->references[$toFormat->id], $fromFormat->id, $fromId, $links, false);
	}

	function deleteAllLinks($fromId, $fromFormat, $toFormat) {
		$links = array(
			array(
				'off' => false,
			)
		);

		$this->modifyLinks($fromFormat->references[$toFormat->id], $fromFormat->id, $fromId, $links);
	}


	/**
	 *
	 * Использует {@link PXDatabase::Query}
	 *
	 * @todo docme (check params documentation)
	 *
	 * @param Mixed             $reference  Unknown
	 * @param PXTypeDescription $format
	 * @param Mixed             $id         Unknown
	 * @param Mixed             $links      Unknown
	 */
	function ModifyLinks($reference, $format, $id, $links, $deleteAll=true) {
		if(!is_array($links) || !sizeof($links)){
			return;
		}
		$audit = PXAuditLogger::getLogger($this->app, $this);

		$linkedFormat = $reference->getOther($format);

		$objectInDB = $this->getObjectById($this->types[$format], $id);
		if (!count($objectInDB)) $this->FatalError('Попытка модифицировать отсутствующий объект');

		if (!$this->user->can(array('write', 'modify'), $this->types[$format], $objectInDB)) {
			$errString = 'Отказано в доступе при попытке модифицировать объект типа <em>'.$format.'</em>';
			$audit->error($errString, $format .'/'. $objectInDB['id']);
			$this->FatalError($errString);
		}
		$tmp = array();
		foreach ($links as $k=>$v) {
			if(is_string($k) && !ctype_digit($k)) {
				$this->FatalError('Попытка SQL-Injection!');
			}
			if (isset($v['on'])) $tmp[] = $k;
		}
		$objectsToLink = $this->getObjectsByIdArray($this->types[$linkedFormat], NULL, $tmp);
		foreach ($objectsToLink as $objectToLink) {
			if (!$this->user->can(array('write', 'modify'), $this->types[$linkedFormat], $objectToLink) && !$this->user->can('linkto', $this->types[$linkedFormat], $objectToLink)) {
				$errString = 'Отказано в доступе при попытке привязать объект типа <em>'.$linkedFormat.'</em> к объекту типа <em>'.$format.'</em>';
				$audit->error($errString, $linkedFormat .'/'. $objectToLink['id']);
				$this->FatalError($errString);
			}
		}

		$permitModifyLinks = true;
		foreach ($this->triggers as $t){
			$permitModifyLinks = $t->getTrigger()->OnBeforeModifyLinks($this, $id, $format, $links, $linkedFormat, $objectInDB, $reference);
			if(!$permitModifyLinks) break;
		}

		if($permitModifyLinks){
			$this->transactionBegin();

			$query = '';
			$keys = implode(', ', array_keys($links));
			if ($linkedFormat === $format) {
				$format .= '_';

				if(!$deleteAll) {
					if (sizeof($links)) {
						$query .= <<<SQL
DELETE FROM {$reference->name}
WHERE {$linkedFormat}id = {$id} AND {$format}id IN ({$keys});
SQL;
					}
				} else {
					$query .= <<<SQL
DELETE FROM {$reference->name}
WHERE {$linkedFormat}id = {$id};
SQL;
				}
			}

			if(!$deleteAll) {
				if(sizeof($links)) {
					$query .= <<<SQL
DELETE FROM {$reference->name}
WHERE {$format}id = {$id} AND {$linkedFormat}id IN ({$keys});
SQL;
				}
			} else {
				$query .= <<<SQL
DELETE FROM {$reference->name}
WHERE {$format}id = {$id};
SQL;
			}

			if(strlen($query)) {
				$this->ModifyingQuery($query, null, null, false);
			}

			$data = array();
			foreach ($links as $k=>$v) {
				do {
					if (!isset($v['on'])) continue;
					$tmp = array();
					if ($linkedFormat . '_' === $format) {
						if ($k == $id) continue;
						$tmp[$linkedFormat.'id'] = min($id, $k);
						$tmp[$format.'id']       = max($id, $k);
					} else {
						$tmp[$linkedFormat.'id'] = $k;
						$tmp[$format.'id']       = $id;
					}

					foreach (array_keys($reference->fields) as $field) {
						$nullObj = NULL;
						if(isset($v['fields'][$field])) {
							$tmp[$field] = $this->_NormalizeObjectAttribute($field, $v['fields'], $reference->fields[$field], $nullObj, $nullObj);
						} else {
							$tmp[$field] = NULL;
						}
					}
					$data[] = $tmp;
				} while(!is_null($v = & $v['next']));
			}
			if (count($data)) {
				$this->ModifyingCopy($reference->name, array_keys(reset($data)), $data);
			}
			$this->clearLinksCache($reference->name);
			$this->transactionCommit();

			foreach ($this->triggers as $t)
				$t->getTrigger()->OnModifyLinks($this, $id, $format, $links, $linkedFormat);
		}
	}

	/**
	 * Нет документации
	 *
	 * @todo docme (check params documentation)
	 * @access public
	 */
	function _NormalizeTable(&$table, $format, $withoutMeta = false) {
		$p = array('db' => &$this, 'format' => $format->id);
		$inMeta = [];
		foreach ($table as $k=>$v) {

			$isMetaPresent = !empty($v['sys_meta']);

			foreach ($format->fields as $fk=>$fv) {
				if (!isset($inMeta[$fk])) { // cache results, cuz lots of rows in table
					$inMeta[$fk] = $fv->storageType->storedInMeta();
				}
				if ($inMeta[$fk] && ($withoutMeta || $isMetaPresent)) {
					$table[$k][$fk] = null;
					continue;
				}

				$table[$k][$fk] = $fv->storageType->normalize($fv, $table[$k], $p);
			}

			$table[$k]['sys_created']  = $this->importDateTime($table[$k]['sys_created']);
			$table[$k]['sys_modified'] = $this->importDateTime($table[$k]['sys_modified']);

			if ($isMetaPresent) {
				$sysMeta = ($table[$k]['sys_meta'] != null)
					? json_decode($table[$k]['sys_meta'], true)
					: [];

				$sysMeta = is_array($sysMeta)
					? $sysMeta
					: [];

				$table[$k]['sys_meta'] = $sysMeta; // нормализуем sys_meta

				// загружаем сохраненные мета-поля, в случае их
				// отсутствия (например работаем в режиме ноды кластера)
				// выставлям поля как если бы был доступ site/htdocs/ai
				foreach($sysMeta as $sysMetaKey => $sysMetaValue) {
					if (empty($table[$k][$sysMetaKey])) {
						$table[$k][$sysMetaKey] = $sysMetaValue;
					}
				}
			}

			if (count($format->childs)) {
				if (!empty($table[$k][OBJ_FIELD_CHILDREN])) {
					// TODO: replace with json_decode
					$table[$k][OBJ_FIELD_CHILDREN] = unserialize($table[$k][OBJ_FIELD_CHILDREN]);
				} else {
					$table[$k][OBJ_FIELD_CHILDREN] = [];
				}
			}
		}
	}

	/**
		Интерфейс к преобразованию SQL выборки к объекту datatype
		@access public
	*/
	public function resultToObject($resTable, $format, $mode) {
		return $this->_PrepareResult($resTable, $format, $mode);
	}

	/**
	 * Нет документации
	 * @access protected
	 * @todo docme (check params documentation)
	 */
	protected function _PrepareResult($resTable, $format, $mode) {
		$mode = (empty($mode)) ? DB_SELECT_TABLE : $mode; // if mode == NULL, function behavior is undefined
		switch (true) {
			case $mode & DB_SELECT_TABLE:
				PXProfiler::begin('PREPARE: '.$format->id.' ['.sizeof($resTable).'x'.sizeof($format->fields).']');

				$this->_NormalizeTable($resTable, $format, !!($mode & DB_SELECT_WITHOUT_EXTERNALS));

				$newTable = array();
				foreach ($resTable as $k=>$v) {
					if ($this->user->can('read', $format, $v)) {
						$newTable[$v['id']] = $v;
					}
					unset($resTable[$k]);
				}

				PXProfiler::end();

				return $newTable;
				break;

			case $mode & DB_SELECT_TREE:
			case $mode & DB_SELECT_FULL_TREE:
				PXProfiler::begin('PREPARE: '.$format->id.' ['.sizeof($resTable).'x'.sizeof($format->fields).']');

				$this->_NormalizeTable($resTable, $format, !!($mode & DB_SELECT_WITHOUT_EXTERNALS));
				$newTable = array();

				foreach ($resTable as $k=>$v) {
					if ($this->user->can('read', $format, $v)) {
						$newTable[$v['id']] = $v;
					}
					unset($resTable[$k]);
				}
				PXProfiler::end();

				PXProfiler::begin('TREE: '.$format->id.' ['.sizeof($newTable).']');

				$parentRefName = !isset($format->fields['pid']) ? 'parent' : 'pid';

				foreach($this->triggers as $t) {
					$t->getTrigger()->OnBeforeBuildTree($this, $format, $newTable, $parentRefName);
				}
				$newTree = $this->buildTree($newTable, 'id', $parentRefName, 'title', !!($mode & DB_SELECT_FULL_TREE));
				PXProfiler::end();

				return $newTree;
				break;

			case DB_SELECT_COUNT:
				reset($resTable[0]);
				return current($resTable[0]);
				break;
		}
	}

	public function buildTree() {
		$args = func_get_args();
		$treeKlass = new ReflectionClass("NLTree");
		return $treeKlass->newInstanceArgs($args);
	}

	function filterByAccess($what, $format, &$table) {
		if (is_array($table)) {
			foreach ($table as $k=>$v) {
				if (!$this->user->can($what, $format, $v)) {
					unset($table[$k]);
				}
			}

		} else {
			$plainTable = $table->toTable();

			foreach ($plainTable as $k=>$v) {
				if (!$this->user->can($what, $format, $v)) {
					unset($plainTable[$k]);
				}
			}

			$table = $this->buildTree($plainTable, $table->_idField, $table->_parentField, $table->_titleField, $table->saveOrphans);
		}
	}

	private function checkForParent($format, &$object) {
		if (!(isset($format->fields['parent']) && $object['parent'])) {
			return;
		}

		$audit = PXAuditLogger::getLogger($this->app, $this);

		$permitCheck = null;
		$parentObject = null;

		foreach ($this->triggers as $t) {
			$permitCheck = $t->getTrigger()->OnBeforeCheckForParent($this, $format, $object, $parentObject);
			if($permitCheck !== null) break;
		}

		if ($permitCheck !== true) {
			if ($permitCheck === null) {
				$parentObject = $this->getObjectById($this->types[$format->parent], $object['parent']);
			}

			if ($permitCheck === false || !count($parentObject)) {
				$errString = 'Попытка переместить объект в несуществующий раздел';
				$audit->error($errString, $format->id .'/'.$object['id']);
				$this->FatalError($errString);
			}
		}

		if ($format->id != $format->parent && !in_array($format->id, $this->app->GetAllowedChildsKeys($format->parent, $parentObject))) {
			$errString = 'Объект данного типа не может быть добавлен в данный раздел';
			$audit->error($errString, $format->parent .'/'. $object['parent']);
			$this->FatalError($errString);
		}
	}

	function importBoolean($string) {
		return $this->dbDriver->importBoolean($string);
	}

	protected function clearObjectTypeCache($format) {
		$this->cache->deleteGroup($format);
		foreach($this->app->references as $ref){
			if($ref->from != $format && $ref->to != $format) {
				continue;
			}
			$this->clearLinksCache($ref->name);
		}
	}

	protected function clearLinksCache($reference) {
		$this->cache->deleteGroup($reference);
	}
}
