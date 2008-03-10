<?
/**
 * Базовый класс описывающий сохранение, получение из запроса
 * (и что-то еще) полей объекта с определеннм storageType
 *
 * Типичный способ обращения к статическим методам класса:
 * PXStorageType::callMethod($method, $field, $object, $param);
 *
 */
class PXStorageType {
	/**
	 * Вызывает нужный метод нужного класса
	 *
	 * @param String             $method
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return Mixed
	 */
	static function callMethod($method, $field, $object, $param=NULL) {
		return PXFieldUtil::callMethod($method, $field, $object, $param, __CLASS__, 'storageType');
	}

	/**
	 * Возвращает заголовок таблицы объектов
	 * возможно с ссылкой на изменение порядка
	 * сортировки объектов
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return String
	 */
	static function objectsSortOrder($field, $object, $param=NULL) {
		return $field->description;
	}

	/**
	 * Возвращает нормированное значение поля из запроса
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return Mixed
	 */
	static function getFromRequest($field, $object, &$param) {
		return $object[$field->name];
	}

	/**
	 * Возвращает нормированное значение
	 * поля перед сохранением объекта в базу
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return Array
	 */
	static function normalizeObjectAttribute($field, $object, &$param) {
		return array(NULL, $param['dbFields'], $param['dbValues']);
	}

	/**
	 * Возвращает условие поиска (SQL) некоего слова
	 * в поле
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return String
	 */
	static function searchLikeQuery($field, $object, &$param) {
		return '';
	}

	/**
	 * Сохраняет файлы из $_FILES в site/htdocs/ai
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return String
	 */
	static function proceedFile($field, $object, &$param) {
		return null;
	}

	/**
	 * Возвращает нормированное значение
	 * поля после выборки объекта из базы
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return Mixed
	 */
	static function normalize($field, $object, &$param) {
		return isset($object[$field->name]) ? $object[$field->name] : NULL;
	}

	/**
	 * Генерит pathname объекта
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return String
	 */
	static function createPathname($field, $object, &$param) {
		return '';
	}

	/**
	 * Вызывается в PXDatabase::_createSelector при составлении
	 * списка полей объекта извлекаемых из базы
	 * Возвращает флаг, извлекать ли поле из базы
	 *
	 * @param PXFieldDescription $field
	 * @param Array              $object
	 * @param Array|NULL         $param
	 *
	 * @return Boolean
	 */
	static function notInDb($field, $object, $param=NULL) {
		return false;
	}
}


/**
 * Строка
 * В базе может храниться как VARCHAR(x), так и TEXT
 *
 * displayType HIDDEN, TEXT, RICHEDIT, DROPDOWN, COLOR, STATIC
 * storageType string
 */
class PXStorageTypeString extends PXStorageType {
	static function objectsSortOrder($field, $object, &$param) {
		$html = '<a ';

		switch($param['order']) {
			case ($field->name.' DESC') :
				$html .= ' class="down"';
				$order =  $field->name;
				$title = '';
				break;

			case $field->name :
				$html .= ' class="up"';
				$order = $field->name.' DESC';
				$title = ' в обратном порядке';
				break;

			default:
				$order =  $field->name;
				$title = '';
				break;
		}

		$html .= ' href="'.$param['table']->_BuildHref($param['dtype'].'_order', $order).'"';
		$html .= ' title="Сортировать по полю &bdquo;'.$field->description.'&ldquo;'.$title.'">'.$field->description.'</a>';

		return $html;
	}

	static function getFromRequest($field, $object, &$param) {
		return (string)$object[$field->name];
	}

	static function normalizeObjectAttribute($field, $object, &$param) {
		$k = $field->name;
		$v = $object[$k];

		if(is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}

		if(is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return array($v, $param['dbFields'], $param['dbValues']);
	}

	static function searchLikeQuery($field, $object, &$param) {
		return $param['formatId'].'.'.$field->name." LIKE '%".$param['word']."%' OR ";
	}
}


/**
 * Целое число
 * В базе может храниться как INT4
 *
 * displayType HIDDEN, TEXT, DROPDOWN, STATIC, PARENTDROPDOWN, SELFPARENTDROPDOWN, IPADDR
 * storageType integer
 */
class PXStorageTypeInteger extends PXStorageType {
	static function getFromRequest($field, $object, &$param) {
		$k = $field->name;
		return (!(is_null($object[$k]) || $object[$k] === '')) ? (int)$object[$k] : NULL;
	}

	static function objectsSortOrder($field, $object, &$param) {
		return PXStorageTypeString::objectsSortOrder($field, $object, $param);
	}

	static function normalizeObjectAttribute($field, $object, &$param) {
		$k = $field->name;

		$v = (is_numeric($object[$k]) || empty($object[$k])) ? $object[$k] : 0;

		if(is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}

		if(is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return array($v, $param['dbFields'], $param['dbValues']);
	}
}

/**
 * Целое число, id объекта
 * В базе может храниться как INT4 PRIMARY KEY
 *
 * displayType HIDDEN, STATIC
 * storageType pk
 */
class PXStorageTypePk extends PXStorageTypeInteger {
	static function objectsSortOrder($field, $object, &$param) {
		return PXStorageType::objectsSortOrder($field, $object, $param);
	}

	static function normalizeObjectAttribute($field, $object, &$param) {
		return PXStorageType::normalizeObjectAttribute($field, $object, $param);
	}
}

/**
 * Число с плавающей запятой
 * В базе может храниться как REAL
 *
 * displayType HIDDEN, TEXT, STATIC
 * storageType float
 */
class PXStorageTypeFloat extends PXStorageType {
	static function getFromRequest($field, $object, &$param) {
		$k = $field->name;
		return (!(is_null($object[$k]) || $object[$k] === '')) ? parseFloat($object[$k]) : NULL;
	}

	static function objectsSortOrder($field, $object, &$param) {
		return PXStorageTypeString::objectsSortOrder($field, $object, $param);
	}

	static function normalizeObjectAttribute($field, $object, &$param) {
		$k = $field->name;

		$v = parseFloat($object[$k]);
		$v = $param['db']->exportFloat($v);

		if(is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}

		if(is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return array($v, $param['dbFields'], $param['dbValues']);
	}
}

/**
 * Логическое поле, флаг
 * В базе может храниться как BOOL
 *
 * displayType HIDDEN, CHECKBOX, STATIC
 * storageType boolean
 */
class PXStorageTypeBoolean extends PXStorageType {
	static function getFromRequest($field, $object, &$param) {
		return (bool)$object[$field->name];
	}

	static function objectsSortOrder($field, $object, &$param) {
		return PXStorageTypeString::objectsSortOrder($field, $object, $param);
	}

	static function normalizeObjectAttribute($field, $object, &$param) {
		$k = $field->name;
		$v = (bool)$object[$k] ? '1' : '0';

		if(is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}

		if(is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return array($v, $param['dbFields'], $param['dbValues']);
	}

	static function normalize($field, $object, &$param) {
		return $param['db']->importBoolean($object[$field->name]);
	}
}

/**
 * Дата и время
 * В базе может храниться как TIMESTAMP
 *
 * displayType HIDDEN, STATIC, TIMESTAMP, TIME, DATE
 * storageType timestamp
 */
class PXStorageTypeTimestamp extends PXStorageType {
	static function normalizeObjectAttribute($field, $object, &$param) {
		$object[$field->name] = $object[$field->name]=== 'null' ? NULL : $param['db']->exportDateTime($object[$field->name]);
		$v = PXStorageTypeString::normalizeObjectAttribute($field, $object, $param);

		return $v;
	}

	static function objectsSortOrder($field, $object, &$param) {
		return PXStorageTypeString::objectsSortOrder($field, $object, $param);
	}

	static function normalize($field, $object, &$param) {
		return $param['db']->importDateTime($object[$field->name]);
	}

	static function createPathname($field, $object, &$param) {
		if (isset($object[$field->name])) {
			return preg_replace("/^(\d{2})\.(\d{2})\.(\d{4})\s(\d{2}):(\d{2}):(\d{2}).*/", "$3$2$1-$4$5", trim($object[$field->name]));
		}
	}
}

/**
 * Сериализованный массив
 * В базе может храниться как TEXT
 *
 * displayType HIDDEN, STATIC, TABLE, LINKTOFILE
 * storageType serialized
 */

class PXStorageTypeSerialized extends PXStorageType {
	static function getFromRequest($field, $object, &$param) {
		$k = $field->name;
		return (is_array($object[$k])) ? $object[$k] : array();
	}

	static function normalizeObjectAttribute($field, $object, &$param) {
		$k = $field->name;

		$v = serialize((is_array($object[$k]) ? $object[$k] : array()));

		if(is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}

		if(is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return array($v, $param['dbFields'], $param['dbValues']);
	}


	static function normalize($field, $object, &$param) {
		return ($object[$field->name]) ? unserialize($object[$field->name]) : $object[$field->name];
	}
}

/**
 * Файл
 * хранится в site/htdocs/ai/ТИП_ДАННЫХ/ИД_ОБЪЕКТА/ИМЯ_ПОЛЯ/ИМЯ_ФАЙЛА
 * например: site/htdocs/ai/news/123034/attach/rules.doc
 *
 * displayType FILE
 * storageType file
 */
class PXStorageTypeFile extends PXStorageType {
	static function proceedFile($field, $object, &$param) {
		$fileArray = $object[$field->name];
		$basedir = BASEPATH.'/site/htdocs/ai/'.$param['format'].'/'.$param['id'].'/'.$field->name.'/';
		$filename = null;

		if (is_array($fileArray)) {
			if (!empty($fileArray['name'])) {
				if(isset($param['allowed']) && is_array($param['allowed'])) {
					$allowed = $param['allowed'];

					$tmp = _TranslitFilename($fileArray['name']);
					$tmp = preg_replace('/\.\w+$/', '', $tmp);

					if(in_array($fileArray['type'], array_keys($allowed))) {
						$filename = $tmp.'.'.$allowed[$fileArray['type']];
					}

				} else {
					$filename = _TranslitFilename($fileArray['name']);
				}

				if(!is_null($filename)) {
					MakeDirIfNotExists($basedir);

					$d = dir($basedir);

					while ($entry = $d->read()) {
						if (is_file($basedir.$entry)) {
							unlink($basedir.$entry);
						}
					}

					copy($fileArray['tmp_name'], $basedir.$filename);
				}
			}

			if (isset($fileArray['remove']) && is_array($fileArray['remove'])) {
				foreach ($fileArray['remove'] as $fileName) {
					if (file_exists($basedir.$fileName)) {
						unlink($basedir.$fileName);
					}
				}
			}
		}
	}

	static function normalize($field, $object, &$param) {
		$fileName = NULL;
		$basedir = 'ai/'.$param['format'].'/'.$object['id'].'/'.$field->name;
		$d = new NLDir(BASEPATH.'/site/htdocs/'.$basedir);

		if ($d->Exist()) {
			while ($entry = $d->Read()) {
				$fileName = $entry;
			}
		}

		return array(
			'filename' => isset($fileName) ? $fileName                  : NULL,
			'fullpath' => isset($fileName) ? '/'.$basedir.'/'.$fileName : NULL,

			'type'     => (isset($fileName) && is_file($d->name.'/'.$fileName)) ? mime_content_type($d->name.'/'.$fileName) : NULL,
			'size'     => (isset($fileName) && is_file($d->name.'/'.$fileName)) ? filesize($d->name.'/'.$fileName)          : 0
		);
	}

	static function notInDb($field, $object, $param=NULL) {
		return true;
	}
}

/**
 * Картинка
 * хранится в site/htdocs/ai/ТИП_ДАННЫХ/ИД_ОБЪЕКТА/ИМЯ_ПОЛЯ/ИМЯ_ФАЙЛА.РАЗРЕШЕННЫЙ_ТИП
 * например: site/htdocs/ai/news/123034/image/bush.gif
 *
 * displayType IMAGE
 * storageType image
 */
class PXStorageTypeImage extends PXStorageTypeFile {
	static function proceedFile($field, $object, &$param, $allowed=ENUM_ALLOWED_IMAGETYPES) {
		$param['allowed'] = unserialize($allowed);
		return PXStorageTypeFile::proceedFile($field, $object, $param);
	}

	static function normalize($field, $object, &$param, $allowed=ENUM_ALLOWED_IMAGETYPES) {
		$allowedImageTypes = array_unique(unserialize($allowed));

		$tmp = glob(BASEPATH.'/site/htdocs/ai/'.$param['format'].'/'.$object['id'].'/'.$field->name.'/*');

		if (!count($tmp)) {
			return;
		}

		if (!in_array(strtolower(substr($tmp[0], -3)), $allowedImageTypes)) {
			return;
		}

		$diskPath = $tmp[0];
		$httpPath = str_replace(BASEPATH.'/site/htdocs', '', $diskPath);
		$httpPath = urlencode($httpPath);
		$httpPath = str_replace('%2F', '/', $httpPath);
		$httpPath = str_replace('+', '%20', $httpPath);

		list($width, $height, ) = getimagesize($diskPath);

		return array(
			'path'   => isset($httpPath) ? $httpPath : NULL,
			'width'  => isset($width)    ? $width    : NULL,
			'height' => isset($height)   ? $height   : NULL
		);
	}
}
/**
 * Флешка
 * хранится в site/htdocs/ai/ТИП_ДАННЫХ/ИД_ОБЪЕКТА/ИМЯ_ПОЛЯ/ИМЯ_ФАЙЛА.swf
 * например: site/htdocs/ai/banner/125846/movie/banner.swf
 *
 * displayType FLASH
 * storageType flash
 */
class PXStorageTypeFlash extends PXStorageTypeImage {
	static function proceedFile($field, $object, &$param) {
		return PXStorageTypeImage::proceedFile($field, $object, $param, ENUM_ALLOWED_FLASHTYPES);
	}

	static function normalize($field, $object, &$param) {
		return PXStorageTypeImage::normalize($field, $object, $param, ENUM_ALLOWED_FLASHTYPES);
	}
}

/**
 * Список файлов
 * хранятся в site/htdocs/ai/ТИП_ДАННЫХ/ИД_ОБЪЕКТА/ИМЯ_ПОЛЯ/ИМЯ_ФАЙЛА
 * например: site/htdocs/ai/news/123034/attach/rules.doc
 *           site/htdocs/ai/news/123034/attach/cases.xls
 *           site/htdocs/ai/news/123034/attach/nude.jpg
 *
 * displayType FILESARRAY
 * storageType filesarray
 */
class PXStorageTypeFilesarray extends PXStorageType {
	static function proceedFile($field, $object, &$param) {
		$varName   = $field->name;
		$basedir   = BASEPATH.'/site/htdocs/ai/'.$param['format'].'/'.$param['id'].'/'.$varName;
		$fileArray = $object[$varName];
		$allowed   = isset($param['allowed']) ? $param['allowed'] : NULL;

		if (is_array($fileArray) && !empty($fileArray['name'][0])) {
			$filename = NULL;

			if(is_array($allowed)) {
				if(in_array($fileArray['type'][0], array_keys($allowed))) {
					MakeDirIfNotExists($basedir);

					$tmp = _TranslitFilename($fileArray['name'][0]);
					$tmp = preg_replace('/\.\w+$/', '', $tmp);

					if(in_array($fileArray['type'][0], array_keys($allowed))) {
						$filename = $basedir.'/'.$tmp.'.'.$allowed[$fileArray['type'][0]];
					}
				}

			} else {
				MakeDirIfNotExists($basedir);
				$filename = $basedir.'/'. _TranslitFilename($fileArray['name'][0]);
			}

			if(!is_null($filename)) {
				copy($fileArray['tmp_name'][0], $filename);
			}
		}

		if (is_array($fileArray) && isset($fileArray['remove']) && is_array($fileArray['remove'])) {
			foreach ($fileArray['remove'] as $fileName) {
				if (file_exists($basedir.'/'.$fileName)) {
					unlink($basedir.'/'.$fileName);
				}
			}
		}

		if (is_array($_FILES) && isset($_FILES[$varName.'_edit']) && is_array($_FILES[$varName.'_edit'])) {
			foreach ($_FILES[$varName.'_edit']['name'] as $k=>$v) {
				if (file_exists($basedir.'/'.$k) && isset($v) && strlen($v)) {
					unlink($basedir.'/'.$k);
					copy($_FILES[$varName.'_edit']['tmp_name'][$k], $basedir.'/'._TranslitFilename($v));
				}
			}
		}
	}


	static function normalize($field, $object, &$param) {
		$filesArray = array();
		$basedir = '/ai/'.$param['format'].'/'.$object['id'].'/'.$field->name.'/';
		$d = new NLDir(BASEPATH.'/site/htdocs'.$basedir);

		if ($d->Exist()) {
			foreach ($d->getFileNames() as $entry) {
				$diskPath = BASEPATH.'/site/htdocs'.$basedir.$entry;
				$httpPath = $basedir.$entry;

				$filesArray[$entry] = array(
					'fullpath' => isset($httpPath) ? $httpPath : NULL,
					'filename' => $entry,
					'size'     => filesize($diskPath),
					'type'     => mime_content_type($diskPath),
					'entry'    => $entry
				);

				unset($httpPath);
				unset($diskPath);
			}
		}

		return $filesArray;
	}

	static function notInDb($field, $object, $param=NULL) {
		return true;
	}
}
/**
 * Список картинок
 * хранятся в site/htdocs/ai/ТИП_ДАННЫХ/ИД_ОБЪЕКТА/ИМЯ_ПОЛЯ/ИМЯ_ФАЙЛА.РАЗРЕШЕННЫЙ_ТИП
 * например: site/htdocs/ai/news/123034/images/1.jpg
 *           site/htdocs/ai/news/123034/images/girl.png
 *           site/htdocs/ai/news/123034/images/dos.jpg
 *
 * displayType IMAGESARRAY
 * storageType imagesarray
 */
class PXStorageTypeImagesarray extends PXStorageTypeFilesarray {
	static function proceedFile($field, $object, &$param, $allowed=ENUM_ALLOWED_IMAGETYPES) {
		$param['allowed'] = unserialize($allowed);
		return PXStorageTypeFilesarray::proceedFile($field, $object, $param);
	}

	static function normalize($field, $object, &$param) {
		$imagesArray = array();
		$basedir = '/ai/'.$param['format'].'/'.$object['id'].'/'.$field->name.'/';

		$d = new NLDir(BASEPATH.'/site/htdocs'.$basedir);

		if ($d->Exist()) {
			foreach ($d->getFileNames() as $entry) {
				$diskPath = BASEPATH.'/site/htdocs'.$basedir.$entry;
				$httpPath = $basedir.$entry;
				list($width, $height, ) = getimagesize($diskPath);

				$imagesArray[$entry] = array(
					'path'   => isset($httpPath) ? $httpPath : NULL,
					'width'  => isset($width)    ? $width    : NULL,
					'height' => isset($height)   ? $height   : NULL,
					'entry'  => $entry
				);

				unset($httpPath);
				unset($width);
				unset($height);
			}
		}

		return $imagesArray;
	}
}
/**
 * Список флэшей
 * хранятся в site/htdocs/ai/ТИП_ДАННЫХ/ИД_ОБЪЕКТА/ИМЯ_ПОЛЯ.НОМЕР.swf
 * например: site/htdocs/ai/postcard/123034/movies.1.swf
 *           site/htdocs/ai/postcard/123034/movies.2.swf
 *           site/htdocs/ai/postcard/123034/movies.3.swf
 *
 * displayType FLASHSARRAY
 * storageType flashsarray
 */
class PXStorageTypeFlashsarray extends PXStorageTypeImagesarray {
	static function proceedFile($field, $object, &$param) {
		return PXStorageTypeImagesarray::proceedFile($field, $object, $param, ENUM_ALLOWED_FLASHTYPES);
	}
}


/**
 * "Виртуальное" поле
 * нигде не хранится, значение берется или из значения по умолчанию ($field->defaultValue)
 * или позже вычисляется в PXOutputHandler'ах
 *
 * displayType STATIC
 * storageType virtual
 */
class PXStorageTypeVirtual extends PXStorageType {
	static function normalize($field, $object, &$param) {
		return (!is_null($field->defaultValue)) ? $field->defaultValue : NULL;
	}

	static function notInDb($field, $object, $param=NULL) {
		return true;
	}
}

?>
