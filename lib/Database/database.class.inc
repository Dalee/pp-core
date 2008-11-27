<?php
/**Вернуть данные и таблицы в виде массива*/
define('DB_SELECT_TABLE', 0);

/**Вернуть количество строк COUNT()*/
define('DB_SELECT_COUNT', 1);

/**Вернуть данные и таблицы в виде дерева*/
define('DB_SELECT_TREE',  2);

/**Вернуть данные и таблицы в виде LIST*/
define('DB_SELECT_LIST',  3);

/**
 * Класс работы с БД
 */

require_once 'sqladapter.class.inc';
class PXDatabase extends NLSQLDatabaseAdapter {
	/**
	 * PXApplication reference
	 * @var PXApplication
	 */
	var $app;

	/** @var array */
	var $types;

	/** @var PXUser */
	var $user;

	/** @var mixed */
	var $triggers;

	/**
	 * Конструктор класса
	 *
	 * @param PXApplication $app {@link $app} reference to PXApplication
	 */
	function PXDatabase($app, $dbDescriptionName='database') {
		parent::__construct($app->dbDescription[$dbDescriptionName]);

		$this->setUser(new PXUserNull());
		$this->setCache($app->dbDescription[$dbDescriptionName]->cache);

		$this->app      = $app;
		$this->types    = $app->types;
		$this->triggers = NULL;
	}

	function setCache($isCacheOn) {
		$cacheClass = $isCacheOn ? 'ObjectCache' : 'NullCache';
		$this->cache = new $cacheClass('database/level2');
	}

	function setUser($user) {
		$this->user = $user;
	}

	/**
	 * Функуция перегружает функцию родительского класса.
	 * Родительский query возвращает массив в случае успеха и отрицательное целое в случае неуспеха.
	 * Эта функция обрабатывает отрицательное целое и всегда возвращает массив
	 *
	 * @param String $query SQL-query
	 * @param Boolean $donotusecache
	 * @param Array $limitpair
	 * @return Array
	 */
	function query($query, $donotusecache = false, $limitpair = NULL) {
		$resTable = parent::query($query, $donotusecache, $limitpair);

		if ($resTable == ERROR_DB_CANNOTCONNECT) {
			FatalError('Невозможно подсоединиться к БД');

		} elseif ($resTable == ERROR_DB_BADQUERY) {
			FatalError($this->getError());
		}

		return $resTable;
	}

	/**
	 * Загружает триггеры для модулей.
	 *
	 * @todo docme please !
	 */
	function LoadTriggers() {
		if (!is_null($this->triggers)) return;
		$this->triggers = array();
		foreach ($this->app->modules as $name => $module) {
			if (!$module->trigger) continue;

			$file = substr(strtolower($module->class), 8).'.trigger.inc';

			if (file_exists(BASEPATH.'/local/triggers/'.$file)) {
				include_once BASEPATH.'/local/triggers/'.$file;

			} elseif (file_exists(BASEPATH.'/libpp/triggers/'.$file)) {
				include_once BASEPATH.'/libpp/triggers/'.$file;

			} else {
				FatalError('Отсутствует привязанный триггер '.$file.' для модуля '.$name);
			}

			$triggername = 'PXTrigger'.substr($module->class, 8);
			$this->triggers[] = new $triggername($module->settings);
		}
	}

	/**
	 * Загружает каталоги (directories) помеченные в datatypes.xml как schema="sql" && load="automatic"
	 * @see LoadSQLDirectory
	 *
	 * @param Array $directories reference to PXDirectoryDescription objects array
	 */
	function LoadDirectoriesAutomatic(&$directories) {
		foreach ($directories as $k=>$v) {
			if ($v->schema != 'sql')       continue;
			if ($v->load   != 'automatic') continue;
			$this->LoadSQLDirectory($directories[$k]);
		}
	}

	/**
	 * Загружает справочники (directories) помеченные в datatypes.xml как schema="sql"
	 *
	 * @uses LoadSQLDirectory
	 *
	 * @todo docme подробнее
	 *
	 * @param Mixed $format
	 */
	function LoadDirectoriesByType(&$format, $object=NULL) {
		foreach ($format->fields as $k=>$v) {
			if (is_object($v->values) && strtolower(get_class($v->values)) == 'pxdirectorydescription') {
				if (!$format->fields[$k]->values->loaded) {
					switch($v->values->schema) {
						case 'sql':
							$this->LoadSQLDirectory($format->fields[$k]->values, true, $object, $format);
							break;

						case 'xml':
							$this->app->LoadXMLDirectory($format->fields[$k]->values, true);
							break;
					}
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
	 * @param Mixed $where
	 * @param Mixed $object
	 * @param Mixed $format
	 */
	function ParseWhereTemplate($where, $object, $format) {
		if(strpos($where, 'THIS.') !== false) {
			if(is_array($object)) {
				preg_match_all('/THIS\.([A-Z0-9]+)/', $where, $backhref);

				foreach($backhref[1] as $b) {
					if(array_key_exists(strtolower($b), $object)) {
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
			preg_match_all('/PROPERTY\.([A-Z0-9_]+)/', $where, $backhref);
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
	 * Загружает справочник (directory) по ссылке
	 * Использует {@link PXDatabase::_GetData()} для загрузки каталогов из базы данных
	 *
	 * @param PXDirectoryDescription $directory
	 */
	function LoadSQLDirectory(&$directory, $addDefault = true, $object=NULL, $format=NULL) {
		$location = $directory->location;

		$location = $this->ParseWhereTemplate($location, $object, $format);
		$format = $this->types[$directory->source];
        
	        /*
	         * fixme: check this code please
	         * looks like it's totally wrong!
	        */

		foreach($format->fields as $field) {
			if(!$field->listed && $field->name !== 'id') {
				unset($format->fields[$field->name]);
			}
		}

		$directory->values = $this->_GetData($format, DB_SELECT_TABLE, $location);
		$directory->SetLoaded($addDefault);
	}

	/**
	 * Конструирует и исполняет запрос в БД, возвращает массив с данными
	 * Использует {@link NLSQLDatabaseAdapter::Query()} для запросов
	 *
	 * @todo docme (проверить параметры)
	 *
	 * @param PXTypeDescription $format
	 * @param Integer $mode         request mode. Default = {@link DB_SELECT_TABLE}
	 * @param String  $where        raw SQL WHERE clause
	 * @param Array|NULL $limit     NULL or array(LIMIT,OFFSET)
	 * @return Array|NULL           results of {@link NLSQLDatabaseAdapter::Query()}
	 */
	function _GetData($format, $mode, $where=NULL, $limit=NULL, $order=NULL) {
		PXProfiler::begin('GET DATA: '.$format->id.' ['.$where.']-['.(is_array($limit)?implode('.', $limit):'').']-['.$order.']');

		if ($mode == DB_SELECT_COUNT) {
			$selector = 'count('.$format->id.'.id)';

		} else {
			$selector = $this->_createSelector($format);

			if ($format->id != DT_USER) {
				$selector .= ', '.$format->id.'.sys_owner';
			}
		}
		$query = 'SELECT '.$selector.' FROM '.$format->id;

		if (!is_null($where))         $query .= ' WHERE '.$where;
		if ($mode != DB_SELECT_COUNT) {
			if(!is_null($order)) {
				$query .= ' ORDER BY '.$format->id.'.'.$order;
			} else {
				$query .= ' ORDER BY '.$format->id.'.'.$format->order;
			}
		}

		$usergroups = '';
		if(isset($this->user->groups)) {
			$usergroups = ';groups:'.implode('.', $this->user->groups);
		}

		$queryIden = $this->_getCacheIden($query, $mode, $limit, $usergroups, $this->user->aclType());
		$data = $this->cache->load($queryIden);
		if ($data !== null) {
			PXProfiler::end();
			return $data; 
		}

		$sqlRes = $this->Query($query, false, $limit);
		$data = $this->_PrepareResult($sqlRes, $format, $mode);

		$this->cache->save($queryIden, $data);

		PXProfiler::end();
		return $data;
	}

	function _getCacheIden($query, $mode, $limit, $userGroups, $aclType) {
		$queryIden = $query . ';mode:' . $mode;

		if (is_array($limit)) {
			$queryIden .= ';limit:'.implode('.', $limit);
		}

		$queryIden .= ';groups:'.$userGroups;
		$queryIden .= ';acl:'.$aclType;

		return $queryIden;
	}

	function _createSelector(&$format) {
		$selector = NULL;
		foreach ($format->fields as $k=>$v) {
			if(PXStorageType::callMethod('notInDb', $v, null, array())) {
				continue;
			}

			$selector .= $format->id.'.'.$this->mapFields($k).', ';
		}

		if (count($format->childs)) {
			$selector .= $format->id.'.allowed, ';
		}

		$selector .= $format->id.'.sys_created, '.$format->id.'.sys_modified, '.$format->id.'.sys_order';

		if ($format->id != 'suser') {
			$selector .= ', '.$format->id.'.sys_owner';
		}
		return $selector;
	}

	/**
	 * Возвращает массив данных из БД для заданного типа объектов {@link PXTypeDescription}.
	 * Использует {@link PXDatabase::_GetData()} для запросов.
	 *
	 * @param PXTypeDescription $format
	 * @param Integer $id
	 * @return Array
	 */
	function GetObjectById($format, $id) {
		if (empty($id)) {
			return array();
		}
		if (!is_numeric($id)) {
			FatalError('Попытка SQL-Injection!');
		}
		$data = $this->_GetData($format, DB_SELECT_TABLE, $format->id.'.id = '.$id);
		if (count($data)) {
			return reset($data);
		} else {
			return array();
		}
	}

	/**
	 * Возвращает объекты из БД по массиву id (Integer)
	 * Использует {@link NLSQLDatabaseAdapter::Query()} для запросов
	 *
	 * @param PXTypeDescription
	 * @param String $format        objects status ('TRUE' | 'FALSE')
	 * @param Array $status         array of id's
	 * @param Integer $idArray      request mode. Default = {@link DB_SELECT_TABLE}
	 * @return Array
	 */
	function GetObjectsByIdArray($format, $status, $idArray, $mode=DB_SELECT_TABLE, $order=NULL) {
		foreach ($idArray as $id) {
			if (!is_numeric($id)) {
				FatalError('Попытка SQL-Injection!');
			}
		}
		if (count($idArray)) {
			$whereClause = $format->id.'.id IN ('.implode(', ', $idArray).')';
			$whereClause .= !is_null($status) ? ' AND '.$format->id.'.status='.$this->TrueStatusString($status) : NULL;
			return $this->_GetData($format, $mode, $whereClause, null, $order);
		} else {
			return $mode === DB_SELECT_TABLE ? array() : 0;
		}
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
	function GetObjects($format, $status, $mode=DB_SELECT_TABLE, $order=NULL) {
		$whereClause = !is_null($status) ? $format->id.'.status='.$this->TrueStatusString($status) : NULL;
		return $this->_GetData($format, $mode, $whereClause, null, $order);
	}

	/**
	 * Возвращает массив данных из БД для заданного типа объектов {@link PXTypeDescription}.
	 * Использует {@link PXDatabase::_GetData()} для запросов.
	 *
	 * @param PXTypeDescription $format
	 * @param String $status    objects status ('TRUE' | 'FALSE')
	 * @param Integer $limit    database SELECT LIMIT value
	 * @param Integer $offset   database SELECT OFFSET value
	 * @param Integer $mode     request mode, default = {@link DB_SELECT_TABLE}
	 * @return Array
	 */
	function GetObjectsLimited($format, $status, $limit, $offset, $mode=DB_SELECT_TABLE, $order=NULL) {
		$whereClause = !is_null($status) ? $format->id.'.status='.$this->TrueStatusString($status) : NULL;
		return $this->_GetData($format, $mode, $whereClause, array($limit, $offset), $order);
	}

	/**
	 * Возвращает массив из БД для заданного типа объектов {@link PXTypeDescription}.
	 * Использует {@link PXDatabase::_GetData()} для запросов.
	 *
	 * @param PXTypeDescription $format
	 * @param String $status    objects status ('TRUE' | 'FALSE')
	 * @param Integer $parent   parent id (numeric or NULL)
	 * @param Integer $mode     request mode, default = {@link DB_SELECT_TABLE}
	 * @return Array
	 */
	function GetObjectsByParent($format, $status, $parent, $mode=DB_SELECT_TABLE, $order=NULL) {
		if (is_numeric($parent)) {
			$whereClause = $format->id.'.parent = '.$parent;
		} elseif (is_null($parent)) {
			$whereClause = $format->id.'.parent IS NULL';
		} elseif (is_array($parent)) {
			$whereClause = $format->id.'.parent IN ('.implode(',', $parent).')';
		} else {
			FatalError('Попытка SQL-Injection!');
		}
		if (!is_null($status)) $whereClause .= ' AND '.$format->id.'.status='.$this->TrueStatusString($status);
		return $this->_GetData($format, $mode, $whereClause, null, $order);
	}

	/**
	 * Возвращает массив из БД для заданного типа объектов {@link PXTypeDescription}.
	 * Использует {@link PXDatabase::_GetData()} для запросов.
	 *
	 * @param PXTypeDescription $format
	 * @param String $status    objects status ('TRUE' | 'FALSE')
	 * @param Integer $parent   parent id
	 * @param Integer $limit    database SELECT LIMIT value
	 * @param Integer $offset   database SELECT OFFSET value
	 * @param Integer $mode     request mode, default = {@link DB_SELECT_TABLE}
	 * @return Array
	 */
	function GetObjectsByParentLimited($format, $status, $parent, $limit, $offset, $mode=DB_SELECT_TABLE, $order=NULL) {
		if (!is_numeric($parent)) {
			FatalError('Попытка SQL-Injection!');
		}
		$whereClause = $format->id.'.parent = '.$parent;
		if (!is_null($status)) $whereClause .= ' AND '.$format->id.'.status='.$this->TrueStatusString($status);
		return $this->_GetData($format, $mode, $whereClause, array($limit, $offset), $order);
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
	function GetObjectsByWhere($format, $status, $whereClause, $mode=DB_SELECT_TABLE, $order=NULL) {
		if (!is_null($status)) $whereClause .= ' AND '.$format->id.'.status='.$this->TrueStatusString($status);
		return $this->_GetData($format, $mode, $whereClause, null, $order);
	}

	/**

	 * Возвращает массив из БД для заданного типа объектов {@link PXTypeDescription}.
	 * Использует {@link PXDatabase::_GetData()} для запросов.
	 *
	 * @param PXTypeDescription $format
	 * @param String $status    objects status ('TRUE' | 'FALSE')
	 * @param String $field     field name
	 * @param String $value     field value
	 * @param Integer $limit    database SELECT LIMIT value
	 * @param Integer $offset   database SELECT OFFSET value
	 * @param Integer $mode     request mode, default = {@link DB_SELECT_TABLE}
	 * @return Array
	 */
	function GetObjectsByWhereLimited($format, $status, $whereClause, $limit, $offset, $mode=DB_SELECT_TABLE, $order=NULL) {
		if (!is_null($status)) $whereClause .= ' AND '.$format->id.'.status='.$this->TrueStatusString($status);
		return $this->_GetData($format, $mode, $whereClause, array($limit, $offset), $order);
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
	function GetObjectsByField($format, $status, $field, $value, $mode=DB_SELECT_TABLE, $order=NULL) {
		$value = $this->EscapeString($value);
		if (is_array($field)) {
			$whereClause = array();
			foreach ($field as $k => $v) {
				$v = $this->EscapeString($v);
				$whereClause[] = $format->id.'.'.$k.' = '."'".$v."'";
			}
			$whereClause = implode($whereClause, ' AND ');
		} else {
			$whereClause = $format->id.'.'.$field.' = '."'".$value."'";
		}
		if (!is_null($status)) $whereClause .= ' AND '.$format->id.'.status='.$this->TrueStatusString($status);
		return $this->_GetData($format, $mode, $whereClause, null, $order);
	}

	/**
	 * Возвращает массив из БД для заданного типа объектов {@link PXTypeDescription}.
	 * Использует {@link PXDatabase::_GetData()} для запросов.
	 *
	 * @param PXTypeDescription $format
	 * @param String $status    objects status ('TRUE' | 'FALSE')
	 * @param String $field     field name
	 * @param String $value     field value
	 * @param Integer $limit    database SELECT LIMIT value
	 * @param Integer $offset   database SELECT OFFSET value
	 * @param Integer $mode     request mode, default = {@link DB_SELECT_TABLE}
	 * @return Array
	 */
	function GetObjectsByFieldLimited($format, $status, $field, $value, $limit, $offset, $mode=DB_SELECT_TABLE, $order=NULL) {
		$value = $this->EscapeString($value);
		if (is_array($field)) {
			$whereClause = array();
			foreach ($field as $k => $v) {
				$v = $this->EscapeString($v);
				$whereClause[] = $format->id.'.'.$k.' = '."'".$v."'";
			}
			$whereClause = implode($whereClause, ' AND ');
		} else {
			$whereClause = $format->id.'.'.$field.' = '."'".$value."'";
		}

		if (!is_null($status)) {
			$whereClause .= ' AND '.$format->id.'.status='.$this->TrueStatusString($status);
		}

		return $this->_GetData($format, $mode, $whereClause, array($limit, $offset), $order);
	}

	/** @see GetObjectsByField */
	function GetObjectsByFieldLike($format, $status, $field, $value, $mode=DB_SELECT_TABLE, $order=NULL) {
		$field = $this->EscapeString($field);
		$value = $this->EscapeString($value);
		$whereClause = $format->id.'.'.$field.' ILIKE '."'%".$value."%'";
		if (!is_null($status)) $whereClause .= ' AND '.$format->id.'.status='.$this->TrueStatusString($status);
		return $this->_GetData($format, $mode, $whereClause, null, $order);
	}

	/** @see GetObjectsByFieldLimited */
	function GetObjectsByFieldLikeLimited($format, $status, $field, $value, $limit, $offset, $mode=DB_SELECT_TABLE, $order=NULL) {
		$field = $this->EscapeString($field);
		$value = $this->EscapeString($value);
		$whereClause = $format->id.'.'.$field.' ILIKE '."'%".$value."%'";
		if (!is_null($status)) $whereClause .= ' AND '.$format->id.'.status='.$this->TrueStatusString($status);
		return $this->_GetData($format, $mode, $whereClause, array($limit, $offset), $order);
	}

	/** @see GetObjectsByField */
	function GetObjectsByFieldNotEmpty($format, $status, $field, $mode=DB_SELECT_TABLE, $order=NULL) {
		$field = $this->EscapeString($field);
		$whereClause = 'char_length('.$format->id.'.'.$field.') > 0';
		if (!is_null($status)) $whereClause .= ' AND '.$format->id.'.status='.$this->TrueStatusString($status);
		return $this->_GetData($format, $mode, $whereClause, null, $order);
	}

	/** @see GetObjectsByFieldLimited */
	function GetObjectsByFieldNotEmptyLimited($format, $status, $field, $limit, $offset, $mode=DB_SELECT_TABLE, $order) {
		$field = $this->EscapeString($field);
		$whereClause = 'char_length('.$format->id.'.'.$field.') > 0';
		if (!is_null($status)) $whereClause .= ' AND '.$format->id.'.status='.$this->TrueStatusString($status);
		return $this->_GetData($format, $mode, $whereClause, array($limit, $offset), $order);
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
	function GetObjectsBySearchWord($format, $status, $word, $mode=DB_SELECT_TABLE, $order=NULL) {
		$word = $this->EscapeString($word);
		$whereClause = '';
		foreach ($format->fields as $k=>$v) {
			$whereClause .= PXStorageType::callMethod('searchLikeQuery', $v, array(), array('formatId'=>$format->id, 'word'=>$word));
		}
		$whereClause = substr($whereClause, 0, -4);
		if(!strlen($whereClause)) {
			return $mode === DB_SELECT_TABLE ? array() : 0;
		}
		if (!is_null($status)) $whereClause .= ' AND '.$format->id.'.status='.$this->TrueStatusString($status);
		return $this->_GetData($format, $mode, $whereClause, null, $order);
	}

	/**
	 * Возвращает массив данных из БД для заданного типа объектов {@link PXTypeDescription}.
	 * Производит поиск $world по ВСЕМ полям указанным в $format.
	 * Использует {@link PXDatabase::_GetData()} для запросов.
	 *
	 * @param PXTypeDescription $format
	 * @param String $status    objects status ('TRUE' | 'FALSE')
	 * @param String $word      search word
	 * @param Integer $limit    database SELECT LIMIT value
	 * @param Integer $offset   database SELECT OFFSET value
	 * @param Integer $mode     request mode, default = {@link DB_SELECT_TABLE}
	 * @return Array
	 */
	function GetObjectsBySearchWordLimited($format, $status, $word, $limit, $offset, $mode=DB_SELECT_TABLE, $order=NULL) {
		$word = $this->EscapeString($word);
		$whereClause = '';
		foreach ($format->fields as $k=>$v) {
			$whereClause .= PXStorageType::callMethod('searchLikeQuery', $v, array(), array('formatId'=>$format->id, 'word'=>$word));
		}
		$whereClause = substr($whereClause, 0, -4);
		if(!strlen($whereClause)) {
			return $mode === DB_SELECT_TABLE ? array() : 0;
		}
		if (!is_null($status)) $whereClause .= ' AND '.$format->id.'.status='.$this->TrueStatusString($status);
		return $this->_GetData($format, $mode, $whereClause, array($limit, $offset), $order);
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
	 * @see LoadTriggers
	 * @see ModifyingQuery
	 *
	 * @param PXTypeDescription $format
	 * @param Array $object     array of assoaciative arrays 'db_field_name'=>'value'
	 */
	function AddContentObject($format, $object) {
		$audit = PXAuditLogger::getLogger($this->app, $this);
		if (!$this->user->can('write', $format, $object)) {
			$errString = 'Отказано в доступе при попытке создать объект типа <em>'.$format->id.'</em>';
			$audit->error($errString, $format->id .'/0');
			FatalError($errString);
		}
		if (isset($format->fields['parent']) && $object['parent']) {
			$parent = $this->GetObjectById($this->types[$format->parent], $object['parent']);
			if (!count($parent)) {
				$errString = 'Попытка добавить объект в несуществующий раздел';
				$audit->error($errString, $format->parent .'/'. $object['parent']);
				FatalError($errString);
			}
			if ($format->id != $format->parent && !in_array($format->id, $this->app->GetAllowedChildsKeys($format->parent, $parent))) {
				$errString = 'Объект данного типа не может быть добавлен в данный раздел';
				$audit->error($errString, $format->parent .'/'. $object['parent']);
				FatalError($errString);
			}
		}
		$dbFields = array();
		$dbValues = array();
		foreach ($format->fields as $k=>$v) {
			if ($k === 'pathname') {
				$nullObject = NULL;
				$object[$k] = $this->_NormalizeObjectPathName($format, $object, $nullObject);
			}

			$this->_NormalizeObjectAttribute($k, $object, $v, $dbFields, $dbValues);
		}
		// sys fields
		$dbFields[] = 'sys_created';
		$dbFields[] = 'sys_modified';
		$dbValues[] = '##now##';
		$dbValues[] = '##now##';
		if ($format->id != DT_USER) {
			$dbFields[] = 'sys_owner';
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
			$dbValues[] = serialize($_children);
		}

		$this->LoadTriggers();
		$permitAdd = true;
		$newId = 0;
		if(count($this->triggers)){
			foreach ($this->triggers as $t){
				$permitAdd = $t->OnBeforeAddObject($this, $newId, $format, $object, $dbFields, $dbValues);
				if(!$permitAdd) break;
			}
		}

		if($permitAdd){
			$newId = $this->InsertObject($format->id, $dbFields, $dbValues);
			if ($newId < 0) {
				$errString = $this->GetError();
				$audit->error($errString, $format->id .'/'.getFromArray($object, 'parent'));
				FatalError($errString);
			}
			$this->ModifyingQuery('UPDATE '.$format->id.' SET sys_order = id WHERE id = '.$newId);
			foreach ($format->fields as $k=>$v) {
				PXStorageType::callMethod('proceedFile', $v, $object, array('id'=>$newId, 'format'=>$format->id));
			}

			if (count($this->triggers))
				foreach ($this->triggers as $t)
					$t->OnAddObject($this, $newId, $format, $object, $dbFields, $dbValues);

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
		if (isset($format->fields['parent'])) {
			$parentIds = array_unique(GetColFromTable($objectsArray, 'parent'));
			foreach ($parentIds as $parentId) {
				$objectInDB = $this->GetObjectById($this->types[$format->parent], $parentId);
				if (!count($objectInDB)) FatalError('Попытка добавить объект в несуществующий раздел');
				if ($format->id != $format->parent && !in_array($format->id, $this->app->GetAllowedChildsKeys($format->parent, $objectInDB))) {
					FatalError('Объект данного типа не может быть добавлен в данный раздел');
				}
				if (!$this->user->can('write', $format, array('id' => 0, 'parent' => $parentId))) {
					$errString = 'Отказано в доступе при попытке создать объект типа <em>'.$format->id.'</em>';
					$audit->error($errString, $format->id .'/0');
					FatalError($errString);
				}
			}
		}

		$copyCols  = array();
		$copyTable = array();
		$idx       = 0;
		$lastOrder = current(current($this->Query('SELECT max(sys_order) FROM '.$format->id, TRUE)));

		$pathnames = array();
		$nullObj = null;

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

			if ($idx == 0) $copyCols[] = 'sys_order';
			$copyTable[$idx][] = ++$lastOrder;

			// sys fields
			if ($idx == 0) {
				$copyCols[] = 'sys_created';
				$copyCols[] = 'sys_modified';
			}

			$copyTable[$idx][] = '##now##';
			$copyTable[$idx][] = '##now##';

			if ($format->id != DT_USER) {
				if($idx == 0) {
					$copyCols[] = 'sys_owner';
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

				$copyTable[$idx][] = serialize($_children);
			}

			$idx++;
		}
		$res = $this->ModifyingCopy($format->id, $copyCols, $copyTable);
		$audit->info('Добавлено '.$idx.' объектов', $format->id);
		return $res;
		// TRIGGER ?
	}

	/**
	 * Изменяет хранящийся в БД объект
	 * Работает по аналогии с {@link PXDatabase::AddContentObject()}
	 *
	 * @see AddContentObject
	 *
	 * @param PXTypeDescription $format
	 * @param Array $object
	 */
	function ModifyContentObject($format, $object, $preserveHidden = false) {
		$audit = PXAuditLogger::getLogger($this->app, $this);
		if (!$this->user->can('write', $format, $object)) {
			$errString = 'Отказано в доступе при попытке модифицировать объект типа <em>'.$format->id.'</em>';
			$audit->error($errString, $format->parent .'/'. $object['parent']);
			FatalError($errString);
		}

		$objectInDB = $this->GetObjectById($format, $object['id']);
		if (!count($objectInDB)) {
			$errString = 'Попытка модифицировать отсутствующий объект';
			$audit->error($errString, $format->id .'/'.$object['id']);
			FatalError($errString);
		}
		if (!$this->user->can('write', $format, $objectInDB)) {
			$errString = 'Отказано в доступе при попытке модифицировать объект типа <em>'.$format->id.'</em>';
			$audit->error($errString, $format->parent .'/'. $object['parent']);
			FatalError($errString);
		}
		if (isset($format->fields['parent']) && $object['parent']) {
			$parentObject = $this->GetObjectById($this->types[$format->parent], $object['parent']);
			if (!count($parentObject)) {
				$errString = 'Попытка переместить объект в несуществующий раздел';
				$audit->error($errString, $format->id .'/'.$object['id']);
				FatalError($errString);
			}
			if ($format->id != $format->parent && !in_array($format->id, $this->app->GetAllowedChildsKeys($format->parent, $parentObject))) {
				$errString = 'Объект данного типа не может быть перемещен в данный раздел';
				$audit->error($errString, $format->id .'/'.$object['id']);
				FatalError($errString);
			}
		}
		$dbFields = array();
		$dbValues = array();
		foreach ($format->fields as $k=>$v) {
			if ($k === 'pathname') {
				$nullObject = NULL;
				$object[$k] = $this->_NormalizeObjectPathName($format, $object, $nullObject);
			}

			$isContinue = PXDisplayType::callMethod('preModifyObject', $v, $object, array('preserveHidden' => $preserveHidden, 'objectInDB' => $objectInDB));

			if ($isContinue) {
				continue;
			}

			//update fields, but skip not set values (keep original)
			if (array_key_exists($k, $object)) {
				$this->_NormalizeObjectAttribute($k, $object, $v, $dbFields, $dbValues);
			}
		}

		$this->LoadTriggers();
		$permitModify = true;
		if (count($this->triggers)){
			foreach ($this->triggers as $t){
				$permitModify = $t->OnBeforeModifyObject($this, $object['id'], $format, $object, $dbFields, $dbValues, $objectInDB);
				if(!$permitModify) break;
			}
		}
		if($permitModify){
			$dbFields[] = 'sys_modified';
			$dbValues[] = '##now##';
			$result = $this->UpdateObjectByID($format->id, $object['id'], $dbFields, $dbValues);
			if ($result <  0) {
				$audit->error($this->GetError(), $format->id .'/'. $object['id']);
				FatalError($this->GetError());
			}
			if ($format->struct == 'tree' && isset($objectInDB['parent']) && $objectInDB['parent'] != $object['parent']) {
				$childs        = $this->GetObjectsByParent($format, NULL, $objectInDB['parent'], DB_SELECT_TREE);
				$objectsToMove = $childs->GetDescendants($object['id']);
				$query = 'UPDATE '.$format->id.' SET parent='.(is_null($object['parent']) ? 'NULL' : $object['parent']).' WHERE id IN ('.implode(', ', $objectsToMove).')';
				$this->ModifyingQuery($query);

				if(isset($format->pid)) {
					$query = 'UPDATE '.$format->id.' SET pid=NULL WHERE id='.$object['id'];
					$this->ModifyingQuery($query);
				}

			}
			foreach ($format->fields as $k=>$v) {
				PXStorageType::callMethod('proceedFile', $v, $object, array('id'=>$object['id'], 'format'=>$format->id));
			}
			$this->LoadTriggers();
			if (count($this->triggers))
				foreach ($this->triggers as $t)
					$t->OnModifyObject($this, $object['id'], $format, $object, $dbFields, $dbValues, $objectInDB);

			$audit->info('Объект изменен', $format->id .'/'. $object['id']);
		}
		return NULL;
	}

	/**
	 * Изменяет системные параметры объекта хранящегося в БД ('allowed', 'sys_accessmod', 'sys_accessput')
	 * Использует {@link PXDatabase::UpdateObjectByID} для доступа к объектам.
	 *
	 * @see OBJ_FIELD_CHILDREN
	 *
	 * @param PXTypeDescription $format
	 * @param Array             $object
	 */
	function ModifyObjectSysVars($format, $object) {
		$audit = PXAuditLogger::getLogger($this->app, $this);
		$objectInDB = $this->GetObjectById($format, $object['id']);
		if (!count($objectInDB)) FatalError('Попытка модифицировать отсутствующий объект');
		if (!$this->user->can('write', $format, $object)) {
			$errString = 'Отказано в доступе при попытке модифицировать объект типа <em>'.$format->id.'</em>';
			$audit->error($errString, $format->parent .'/'. $object['parent']);
			FatalError($errString);
		}
		$dbFields = array();
		$dbValues = array();
		if (isset($object[OBJ_FIELD_CHILDREN])) {
			$dbFields[] = OBJ_FIELD_CHILDREN;
			$dbValues[] = serialize($object[OBJ_FIELD_CHILDREN]);
		}
		if (count($dbFields)) {
			$result = $this->UpdateObjectByID($format->id, $object['id'], $dbFields, $dbValues);
			if ($result <  0) {
				FatalError($this->GetError());
			}
			$audit->info('Изменены системные параметры объекта', $format->id .'/'. $object['id']);
			// TRIGGER ?
		}
	}

	/**
	 * Удаляет объект (и его childrens) из БД.
	 * Использует {@link PXDatabase::ModifyingQuery} для удаления.
	 * Перед и после удаления вызываются триггеры. Если хотя бы один OnBeforeDeleteObject
	 * триггер вернёт false, удаление не произойдёт.
	 *
	 * @see LoadTriggers
	 *
	 * @param PXTypeDescription $format
	 * @param Array             $objectId   object id
	 */
	function DeleteContentObject($format, $objectId) {
		$audit = PXAuditLogger::getLogger($this->app, $this);
		$objectInDB = $this->GetObjectById($format, $objectId);
		if (!count($objectInDB)) FatalError('Попытка удалить отсутствующий объект');
		if (!$this->user->can('write', $format, $objectInDB)) {
			$errString = 'Отказано в доступе при попытке удалить объект типа <em>'.$format->id.'</em>';
			$audit->error($errString, $format->parent .'/'. $objectInDB['id']);
			FatalError($errString);
		}

		if ($format->struct == 'tree' && isset($format->fields['pid']) && isset($this->types[$format->parent])) {
			$children        = $this->GetObjectsByParent($format, NULL, $objectInDB['parent'], DB_SELECT_TREE);
			$objectsToDelete = $children->GetDescendants($objectId);
		} else {
			$objectsToDelete = array($objectId);
		}
		$this->LoadTriggers();
		$permitDelete = true;
		if (count($this->triggers)) {
			foreach ($this->triggers as $t) {
				$permitDelete = $t->OnBeforeDeleteObject($this, $objectId, $format, $objectInDB);
				if(!$permitDelete) break;
			}
		}

		if($permitDelete) {
			$query  = 'DELETE FROM '.$format->id.' WHERE id IN ('.implode(', ', $objectsToDelete).')';
			$this->ModifyingQuery($query);

			$d = new NLDir(BASEPATH.'/site/htdocs/ai/'.$format->id.'/'.$objectId.'/');
			$d->Delete();
			if (count($this->triggers))
				foreach ($this->triggers as $t)
					$t->OnDeleteObject($this, $objectId, $format, $objectInDB);

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
		for($i=0; $i<abs($shift);$i++) {
			$this->_moveContentObject($format, $objectId, $direction);
		}
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
	function _moveContentObject($format, $objectId, $direction) {
		$audit = PXAuditLogger::getLogger($this->app, $this);

		$objectInDB = $this->GetObjectById($format, $objectId);
		if (!count($objectInDB)) FatalError('Попытка модифицировать отсутствующий объект');

		if (!$this->user->can('write', $format, $objectInDB)) {
			$errString = 'Отказано в доступе при попытке модифицировать объект типа <em>'.$format->id.'</em>';
			$audit->error($errString, $format->id .'/'. $objectInDB['id']);
			FatalError($errString);
		}

		$swap = (preg_match('#sys_order\s+DESC#i', $format->order));
		$down = $direction == 'down';
		if($swap) {
			$down = !$down;
		}
		$where = $format->id.'.sys_order '.($down ? '>' : '<').' '.$objectInDB['sys_order'];

		if (!empty($format->parent)) {
			$where .= ' AND '.$format->id.'.parent = '.$objectInDB['parent'];
		}

		if ($format->struct == 'tree' && isset($format->fields['pid'])) {
			$where .= ' AND '.$format->id.'.pid';

			$pid = $objectInDB['pid'];
			$where .= (is_null($pid) || !is_numeric($pid)) ? ' IS NULL' : ' = '.$pid;
		}

		$nearObject = $this->GetObjectsByWhereLimited($format, NULL, $where, 1, 0, DB_SELECT_TABLE, 'sys_order '.($down ? '' : 'DESC'));

		$this->LoadTriggers();
		$permitMove = true;
		if (count($this->triggers)){
			foreach ($this->triggers as $t){
				$permitMove = $t->OnBeforeMoveObject($this, $objectId, $format, $objectInDB);
				if(!$permitMove) break;
			}
		}

		if($permitMove) {
			if (sizeof($nearObject)) {
				$nearObject = current($nearObject);

				$this->transactionBegin();
				$this->ModifyingQuery('UPDATE '.$format->id.' SET sys_order='.$objectInDB['sys_order'].' WHERE id='.$nearObject['id']);
				$this->ModifyingQuery('UPDATE '.$format->id.' SET sys_order='.$nearObject['sys_order'].' WHERE id='.$objectInDB['id']);
				$this->transactionCommit();

				if (count($this->triggers))
					foreach ($this->triggers as $t)
						$t->OnMoveObject($this, $objectId, $format, $objectInDB);

				$audit->info('Объект перемещен в списке', $format->id .'/'. $objectId);
			}
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
		list($v, $dbFields, $dbValues) = PXStorageType::callMethod('normalizeObjectAttribute', $field, $object, array('dbFields' => $dbFields, 'dbValues' => $dbValues, 'db'=>$this));

		return $v;
	}

	/**
	 * Нормализует PathName объекта
	 * Использует {@link PXDatabase::ModifyingQuery}
	 *
	 * @param PXTypeDescription $format
	 * @param Array             $object     object id
	 * @param Array             $pathnames
	 */
	function _NormalizeObjectPathName($format, $object, &$pathnames) {
		$url = strtolower(_Translit($object['pathname']));

		// part 1: make url if doesn't set
		if (!$url) {
			foreach ($format->fields as $field) {
				$url = PXStorageType::callMethod('createPathname', $field, $object, array());

				if($url) {
					break;
				}
			}
		}

		if (!$url) $url = strtolower(_Translit(substr(preg_replace('/\&.+?\;/', '', $object['title']), 0, 32)));
		if (!$url) $url = sprintf('%08d', rand(0, 100000000));

		// part 2: triming bad chars
		$url = trim($url);
		for($i=0;$i<strlen($url);$i++) {
			$o = ord($url{$i});

			if ($o != 45 && $o != 46 && ($o < 48 || $o > 57) && ($o < 65 || $o > 90) && $o != 95 && ($o < 97 || $o > 122)) {
				$url{$i} = '_';
			}
		}

		// part 3: validating extention
		if ($format->id !== 'struct') {
			$allowed = array_flip(unserialize(ENUM_ALLOWED_PATHNAMES));
			$dotpos = strrpos($url, '.');

			if (!$dotpos || !isset($allowed[substr($url, ++$dotpos)])) {
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
							$condTables[] = $child;
						}
					}

					$reiteration = (int)$this->IsUniqueColsCombination($condTables, $condCols, $exception);
				}

				if ($reiteration > 0) {
					if (is_numeric(strpos($url, '.'))) {
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
		$query = '';

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
					{$reference->name}.{$linkedFormat}id = {$id}

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
					{$reference->name}.{$format}id = {$id}
SQL;

		$limit = is_numeric($limit) && is_numeric($offset) ? array($limit, $offset) : NULL;

		$resTable = $this->Query($query, false, $limit);
		$retTable = array();
		foreach ($resTable as $element) {
			$title = $element[$linkedFormat.'title'];
			$lid   = $element[$linkedFormat.'id'];
			if ($id == $lid and $linkedFormat . '_' == $format) {
				$lid = $element[$format.'id'];
			}
			unset($element[$linkedFormat.'title']);
			unset($element[$linkedFormat.'id']);
			unset($element[$format.'id']);
			$retTable[$lid] = array(
				'title' => $title,
				'fields' => $element
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

		$resTable = $this->query($query);

		return $resTable[0]['count'];
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
		$audit = PXAuditLogger::getLogger($this->app, $this);
		$linkedFormat = $reference->getOther($format);

		$objectInDB = $this->GetObjectById($this->types[$format], $id);
		if (!count($objectInDB)) FatalError('Попытка модифицировать отсутствующий объект');

		if (!$this->user->can('write', $this->types[$format], $objectInDB)) {
			$errString = 'Отказано в доступе при попытке модифицировать объект типа <em>'.$format.'</em>';
			$audit->error($errString, $format .'/'. $objectInDB['id']);
			FatalError($errString);
		}
		$tmp = array();
		foreach ($links as $k=>$v) {
			if (isset($v['on'])) $tmp[] = $k;
		}
		$objectsToLink = $this->GetObjectsByIdArray($this->types[$linkedFormat], NULL, $tmp);
		foreach ($objectsToLink as $objectToLink) {
			if (!$this->user->can('write', $this->types[$linkedFormat], $objectToLink) && !$this->user->can('linkto', $this->types[$linkedFormat], $objectToLink)) {
				$errString = 'Отказано в доступе при попытке привязать объект типа <em>'.$linkedFormat.'</em> к объекту типа <em>'.$format.'</em>';
				$audit->error($errString, $linkedFormat .'/'. $objectToLink['id']);
				FatalError($errString);
			}
		}
		$this->LoadTriggers();
		$permitModifyLinks = true;
		if (count($this->triggers)){
			foreach ($this->triggers as $t){
				$permitModifyLinks = $t->OnBeforeModifyLinks($this, $id, $format, $links);
				if(!$permitModifyLinks) break;
			}
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
				$this->ModifyingQuery($query);
			}

			$data = array();
			foreach ($links as $k=>$v) {
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
			}
			if (count($data)) {
				$this->ModifyingCopy($reference->name, array_keys(reset($data)), $data);
			}
			$this->transactionCommit();

			if (count($this->triggers))
				foreach ($this->triggers as $t)
					$t->OnModifyLinks($this, $id, $format, $links);
		}
	}

	/**
	 * Нет документации
	 *
	 * @todo docme (check params documentation)
	 * @access public
	 */
	function _NormalizeTable(&$table, $format) {
		foreach ($table as $k=>$v) {
			foreach ($format->fields as $fk=>$fv) {
				$table[$k][$fk] = PXStorageType::callMethod('normalize', $fv, $table[$k], array('db'=>&$this, 'format'=>$format->id));
			}

			$table[$k]['sys_created']  = $this->importDateTime($table[$k]['sys_created']);
			$table[$k]['sys_modified'] = $this->importDateTime($table[$k]['sys_modified']);

			if (count($format->childs)) {
				if (!empty($table[$k][OBJ_FIELD_CHILDREN])) {
					$table[$k][OBJ_FIELD_CHILDREN] = unserialize($table[$k][OBJ_FIELD_CHILDREN]);
				} else {
					$table[$k][OBJ_FIELD_CHILDREN] = array();
				}
			}
			if (isset($format->outputHandler)) {
				$file = strtolower($format->outputHandler).'.outputhandler.inc';
				if (file_exists(BASEPATH.'/local/lib/handlers/'.$file)) {
					include_once BASEPATH.'/local/lib/handlers/'.$file;
				} elseif (file_exists(BASEPATH.'/libpp/lib/handlers/'.$file)) {
					include_once BASEPATH.'/libpp/lib/handlers/'.$file;
				}
				$function = 'PXOutputHandler'.$format->outputHandler;
				if (function_exists($function)) {
					$function($format, $table[$k], $this);
				}
			}
		}
	}

	/**
	 * Нет документации
	 * @access protected
	 * @todo docme (check params documentation)
	 */
	function _PrepareResult($resTable, $format, $mode) {
		switch ($mode) {
			case DB_SELECT_TABLE:
				PXProfiler::begin('PREPARE: '.$format->id.' ['.sizeof($resTable).'x'.sizeof($format->fields).']');

				$this->_NormalizeTable($resTable, $format);
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

			case DB_SELECT_TREE:
				PXProfiler::begin('PREPARE: '.$format->id.' ['.sizeof($resTable).'x'.sizeof($format->fields).']');

				$this->_NormalizeTable($resTable, $format);
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
				$newTree = new NLTree($newTable, 'id', $parentRefName, 'title');

				PXProfiler::end();

				return $newTree;
				break;

			case DB_SELECT_COUNT:
				reset($resTable[0]);
				return current($resTable[0]);
				break;
		}
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

			$table = new NLTree($plainTable, $table->_idField, $table->_parentField, $table->_titleField, true);
		}
	}

}

?>