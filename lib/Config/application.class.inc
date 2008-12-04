<?php
require_once 'Description/db.class.inc';

require_once 'Description/type.class.inc';
require_once 'Description/field.class.inc';

require_once 'Description/directory.class.inc';
require_once 'Description/ref.class.inc';

require_once 'Description/module.class.inc';
require_once 'bindingsQueue.class.inc';

/**
 * PXApplication - главный класс.
 *
 * @subpackage Application
 */
class PXApplication {

	/**
	 * массив коннекторов к базе данных
	 * @var Array
	 */
	var $dbDescription;

	/**
	 * массив описаний datatypes {@link PXTypeDescription}
	 * @var Array
	 */
	var $types;

	/**
	 * массив ссылок между datatypes {@link PXRefDescription}
	 * @var Array
	 */
	var $references;

	/**
	 * массив модулей {@link PXModuleDescription}
	 * @var Array
	 */
	var $modules;

	/**
	 * массив настроек из etc/properties.ini
	 * @var Array
	 */
	var $properties;

	/**
	 * массив правил авторизации из etc/auth.ini
	 * @var Array
	 */
	var $authrules;

	/**
	 * массив описаний каталогов из etc/datatypes.xml
	 * @var Array
	 */
	var $directory;

	/**
	 * биндинги модулей из etc/modules.xml modules/bindings
	 * @var Array
	 */
	var $bindingsQueue;

	/**
	 * Singleton array with caching.
	 *
	 */
	public static function getInstance($projectDir) {
		static $instance = array();

		if (isset($instance[$projectDir])) {
			return $instance[$projectDir];
		}

		/* try to get config from cache */
		$instance[$projectDir] = self::loadFromCache($projectDir);

		/* create new instance, if not cached */
		if (!isset($instance[$projectDir])) {
			$instance[$projectDir] = new PXApplication($projectDir);
		}

		return $instance[$projectDir];
	}

	/**
	 * Конструктор класса
	 *
	 * @param String $projectDir - каталог с конфигурацией (относительно {@link BASEPATH})
	 */
	private function __construct($projectDir) {
		$this->types = array();
		$this->directory = array();

		$this->FillDBDescription(self::parseIniFile($this->findConfigFile('database.ini', $projectDir)));
		$this->FillModules($this->findConfigFile('modules.xml', $projectDir));
		$this->FillDataTypes($this->findConfigFile('datatypes.xml', $projectDir));
		$this->FillDirectories($this->findConfigFile('datatypes.xml', $projectDir));

		$this->properties = self::parseIniFiles($this->findConfigFiles('properties.ini', $projectDir));
		$this->authrules  = self::parseIniFiles($this->findConfigFiles('auth.ini', $projectDir));

		//todo: one file or many lang files?
		$this->fillLang($this->findConfigFile('lang.yaml', $projectDir));

		$this->saveToCache($projectDir);
	}

	private static function parseIniFile($file) {
		return parse_ini_file($file, true);
	}

	private static function parseIniFiles($files) {
		/* we must merge array in reverse order: libpp + local + site,
		 * so local can redefine libpp settings, and site can override local */

		$result = self::parseIniFile(array_pop($files));

		while(count($files)) {
			$tmp = self::parseIniFile(array_pop($files));
			foreach($tmp as $k => $v) {
				if (is_array($v) and isset($result[$k]) and is_array($result[$k])) {
					$result[$k] = array_merge($result[$k], $v);
				} else {
					$result[$k] = $v;
				}
			}
		}
		return $result;
	}

	/**
	 *  Searches for the first file in order - site, local, libpp
	 */
	private static function findConfigFile($filename, $projectDir = BASEPATH) {

		$files = self::findFilesUnless($filename, $projectDir,
		                               create_function('$files', 'return count($files) > 0;'));

		if ($files)
			return $files[0];

		FatalError('Отсутствует файл настроек ' . $filename);
	}

	/**
	 *  Searches for all files at site, local, libpp dirs.
	 */
	private static function findConfigFiles($filename, $projectDir) {
		if ($files = self::findFilesUnless($filename, $projectDir)) {
			return $files;
		}

		FatalError('Отсутствует файл настроек ' . $filename);
	}

	/*
	 * function is_enought($files_array) { <return TRUE when enought files collected> }
	 */
	private static function findFilesUnless($filename, $projectDir = BASEPATH, $is_enough = NULL) {
		$files = array();

		foreach(self::configDirs($projectDir) as $configDir) {

			$path = $configDir . DIRECTORY_SEPARATOR . $filename;

			if (file_exists($path))
				$files[] = $path;

			if ($is_enough && $is_enough($files))
				return $files;
		}

		return $files;
	}

	private static function configDirs($projectDir = BASEPATH) {
		return array($projectDir . DIRECTORY_SEPARATOR . 'site'  . DIRECTORY_SEPARATOR . 'etc',
					 $projectDir . DIRECTORY_SEPARATOR . 'local' . DIRECTORY_SEPARATOR . 'etc',
					 $projectDir . DIRECTORY_SEPARATOR . 'libpp' . DIRECTORY_SEPARATOR . 'etc');
	}

	//TODO:FIXME: REWRITE ME!
	private function fillLang ($filename) {
		function __ReplaceIncludeWhenParseLangYaml($replacement, $indent, $fileName) {
			$string = '';

			if(file_exists($fileName)) {
				$tmp = @file($fileName);

			} elseif(file_exists(BASEPATH . DIRECTORY_SEPARATOR . $fileName)) {
				$tmp = @file(BASEPATH . DIRECTORY_SEPARATOR . $fileName);

			} else {
				FatalError('Не могу найти '.$fileName);
			}

			if(is_array($tmp)) {
				foreach($tmp as $s) {
					if(!strlen(trim($s))) {
						continue;
					}

					$string .= $indent.$s;
				}
			}

			return $string;
		}

		$source = ReadFileToString($filename);

		$source = preg_replace('|^((\s*)include\s+(.+?))$|ime', "__ReplaceIncludeWhenParseLangYaml('$1', '$2', '$3');", $source);   // делаем include
		$source = preg_replace('/\t/',          ' ' ,         $source);   // я люблю табы для читабельности
		$source = preg_replace('/(\r?\n){2,}/', "\n",         $source);   // и лишние переводы строк

		$tmp = tempnam(BASEPATH . DIRECTORY_SEPARATOR . 'var', 'lang');
		WriteStringToFile($tmp, $source);

		// Spyc'у не нравятся мои конфиги, поэтому их стоит немного переделать
		include_once 'spyc/spyc.php';
		$spyc = new Spyc();
		$this->langTree = $spyc->load($tmp);
		unlink($tmp);
	}


	/**
	 * Заполняет массив коннекторов к базам данных.
	 *
	 * @see NLDBDescription, $dbDescription
	 *
	 * @param Array $array - массив с параметрами подключений к базе данных
	 */
	private function FillDBDescription($array) {
		$this->dbDescription = array();

		foreach ($array as $k=>$v) {
			$this->dbDescription[$k] = new NLDBDescription($v);
		}
	}

	/**
	 * Возвращает элемент из ({@link properties}) по ключу или значение по умолчанию.
	 *
	 * @param String $k ключ
	 * @param String $defaultValue - возвращаемое значение по умолчанию
	 */
	public function getProperty($k, $defaultValue=NULL) {
		return (isset($this->properties[$k])) ? $this->properties[$k] : $defaultValue;
	}

	/**
	 * Возвращает ассоциативный массив доступных модулей.
	 *
	 * @param Integer $level	название текущего раздела
	 * @return Array			массив из элементов 'module_name' => $module
	 */
	public function getAvailableModules($level) {
		return array_filter($this->modules, create_function('$module', "return \$module->access <= $level;"));
	}

	/**
	 * Возвращает ассоциативный массив разрешенных детей.
	 *
	 * @param NULL $formatName		не используется
	 * @param Object|Array $object	объект для которого искать детей
	 * @return Array				массив разрешенных детей или пустой массив
	 */
	public function getAllowedChilds($formatName, &$object) {
		if(!isset($this->types[$formatName])) {
			return array();
		}

		return $this->types[$formatName]->allowedChilds($object);
	}

	/**
	 * Возвращает массив наименований разрешенных форматов дочерних объектов.
	 *
	 * @uses getAllowedChilds
	 *
	 * @param NULL			не используется
	 * @param Object|Array	объект для которого искать детей
	 * @return Array		массив наименований разрешенных детей или пустой массив
	 */
	public function getAllowedChildsKeys($formatName, &$object) {
		return array_keys($this->getAllowedChilds($formatName, $object));
	}


	/*
	 * todo: two methods do the same?
	 */
	public function initContentObject($formatName) {
		$object = array();

		foreach ($this->types[$formatName]->fields as $k=>$v) {
			$object[$k] = $v->defaultValue;
		}

		return $object;
	}

	public function InitObject($formatName) {
		return $this->InitContentObject($formatName);
	}

	/**
	 * Заполняет {@link PXApplication::$types} и {@link PXApplication::$references} данными из файла.
	 *
	 * В качестве параметра обычно используется файл etc/datatypes.xml. <br>
	 * XPath /model/datatypes/datatype и /model/references/reference.
	 *
	 * @uses PXFieldDescription
	 * @uses PXDirectoryDescription
	 * @uses PXRefDescription
	 * @uses PXTypeDescription
	 *
	 * @todo Для вывода сообщений об ошибках используется debug ф-ция d2(), это нормально ?
	 *
	 * @param String $fileName	имя файла
	 */
	private function FillDataTypes($fileName) {
		$xml = simplexml_load_file($fileName);
		if (!$xml) {
			FatalError("Ошибка в файле настроек <EM>$fileName</EM>");
		}
		$domDatatypes = $xml->xpath("/model/datatypes/datatype");
		if (!is_array($domDatatypes) || count($domDatatypes) == 0) {
			FatalError("Не определена объектная модель проекта");
		}

		foreach ($domDatatypes as $domDatatype) {
			$type = new PXTypeDescription();
			$attrs = $domDatatype->attributes();
			foreach ($attrs as $name => $value) {
				switch ($name) {
					case 'name':          $type->id            = (string)$value; break;
					case 'description':   $type->title         = utf8_decode((string)$value); break;
					case 'orderby':       $type->order         = (string)$value; break;
					case 'parent':        $type->parent        = (string)$value; break;
					case 'bydefault':     $type->byDefault     = self::_mapByDefaults((string)$value); break;
					case 'struct':        $type->struct        = (string)$value; break;
					case 'outputhandler': $type->outputHandler = (string)$value; break;
					default:            d2("Unknown attr name: " . $name . __LINE__); break;
				}
			}

			if (!isset($type->order)) {
				$type->order = 'sys_order';
			}

			$nodes = $domDatatype->children();
			foreach ($nodes as $node) {
				switch ($node->getName()) {
					case 'attribute':
						$nodeattrs = $node->attributes();
						$field = new PXFieldDescription($nodeattrs, $this);

						if(is_null($field->listed)) {
							$field->listed = (bool) ($field->name === 'title');
						}

						$type->addField($field);
						break;
					default:
						d2("Unknown node name: " . $node->getName());
						break;
				}
			}

			$type->allTypes =& $this->types;
			$this->types[$type->id] = $type;
		}

		foreach ($this->types as $k=>$v) {
			if (!empty($v->parent)) {
				if (isset($this->types[$v->parent])) {
					$this->types[$v->parent]->childs[] = $k;
				} else {
					FatalError("ОШИБКА МОДЕЛИ: {$v->id} не может быть дочерним по отношению к {$v->parent}, описание {$v->parent} отсутствует");
				}
			}
		}

		$domReferences = $xml->xpath("/model/references/reference");

		if (!is_array($domReferences) || count($domReferences) == 0) {
			return;
		}

		foreach ($domReferences as $domReference) {
			$attrs = $domReference->attributes();
			$ref = new PXRefDescription();
			$nodes = $domReference->children();
			foreach ($attrs as $name => $value) {
				switch (trim($name)) {
					case 'name':              $ref->name             = (string)$value; break;
					case 'from':              $ref->from             = (string)$value; break;
					case 'to':                $ref->to               = (string)$value; break;
					case 'restrictby':        $ref->restrictBy       = (string)$value; break;
					case 'filter-to':         $ref->filterTo         = (string)$value; break;
					case 'filter-from':       $ref->filterFrom       = (string)$value; break;
					case 'load':              $ref->load             = (string)$value; break;
				}
			}

			foreach ($nodes as $node) {
				switch ($node->getName()) {
					case 'attribute':
						$nodeattrs = $node->attributes();
						$field = new PXFieldDescription($nodeattrs, $this);

						$ref->addField($field);
						break;
					default:
						d2("Unknown node name in reference: ".$node->getName());
						break;
				}
			}
			$this->references[$ref->name] = $ref;
			if (!isset($this->types[$ref->from]) || !isset($this->types[$ref->to])) {
				FatalError("ОШИБКА МОДЕЛИ: Связь <em>{$ref->name}</em> невозможна, отсутствует один из типов");
			}
			$this->types[$ref->from]->references[$ref->to] = &$this->references[$ref->name];
			$this->types[$ref->to]->references[$ref->from] = &$this->references[$ref->name];
		}
	}

	/**
	 * Заполняет {@link PXApplication::$directory} данными из файла.
	 *
	 * В качестве параметра обычно используется файл etc/datatypes.xml. <br>
	 * XPath /model/directories/directory.
	 *
	 * @uses PXDirectoryDescription
	 *
	 * @param String $fileName	имя файла
	 */
	private function FillDirectories($fileName) {
		$xml = simplexml_load_file($fileName);

		if (!$xml) {
			FatalError("Ошибка в файле настроек <EM>$fileName</EM>");
		}
		$domDirectories = $xml->xpath("/model/directories/directory");
		foreach ($domDirectories as $domDirectory) {
			$name = (string)$domDirectory['name'];
			if (!isset($this->directory[$name])) {
				$this->directory[$name] = new PXDirectoryDescription($name);
			}
			$curDir = &$this->directory[$name];
			$curDir->load         = (string)$domDirectory['load'];
			$curDir->schema       = (string)$domDirectory['schema'];
			$curDir->source       = (string)$domDirectory['source'];
			$curDir->sourceDir    = dirname($fileName).DIRECTORY_SEPARATOR;

			$displayfield = trim((string)$domDirectory['displayfield']);
			$curDir->displayField = strlen($displayfield) ? $displayfield : 'title';

			$location = (string)$domDirectory['location'];
			$curDir->location = ($location != '') ? $location : NULL;

			if ($curDir->load == 'automatic' && $curDir->schema == 'xml') {
				$this->LoadXMLDirectory($curDir, false);
			}
		}
	}

	/**
	 * Загружает справочник (directory) по ссылке
	 * Использует {@link PXDatabase::_GetData()} для загрузки каталогов из базы данных
	 *
	 * @param PXDirectoryDescription $directory
	 */
	public function LoadXMLDirectory(&$directory, $addDefault = true) {
		$file          = $directory->sourceDir.$directory->source;
		$xpathSelector = $directory->location;

		$xml = simplexml_load_file($file);

		if (!$xml){
			FatalError("Ошибка в файле справочника <EM>$file</EM>");
		}
		$results = $xml->xpath($xpathSelector);
		if ($results) {
			$nodes = $results[0]->children();

			foreach ($nodes as $node) {
				$directory->values[(string)$node["id"]] = array(
					'id' => (string)$node["id"],
					'title' => utf8_decode((string)$node)
				);
				$attrs = $node->attributes();
				foreach ($attrs as $name => $value) {
					if ($name == 'id') continue;
					$directory->values[(string)$node["id"]][$name] = utf8_decode((string)$value);
				}
			}
			$directory->SetLoaded();
		}

		$directory->SetLoaded($addDefault);
	}

	/**
	 * Заполняет {@link PXApplication::$modules} данными из файла.
	 *
	 * В качестве параметра обычно используется файл etc/modules.xml. <br>
	 * XPath /modules/module
	 *
	 * @uses PXModuleDescription
	 *
	 * @param String $fileName имя файла
	 */
	private function FillModules($fileName) {
		$xml = simplexml_load_file($fileName);

		if (!$xml) {
			FatalError("Ошибка в файле настроек <em>$fileName</em>");
		}

		$domModules = $xml->xpath("/modules//module");

		if (!$domModules || empty($domModules)) {
			FatalError("Не определены модули проекта");
		}

		foreach($domModules as $domModule) {
			$module = new PXModuleDescription();
			$module->setAttrs($domModule->attributes());
			$module->lookPackage($xml);

			$nodes = $domModule->children();
			foreach ($nodes as $name => $value) {
				$module->settings[$name][] = utf8_decode($value);
			}

			foreach ($module->settings as $k=>$v) {
				if (count($v) == 1) {
					$module->settings[$k] = $v[0];
				}
			}

			$this->setModules($module);
		}


		$domBindings = $xml->xpath('/modules/bindings');
		$this->bindingsQueue = new PXBindingsQueue();

		foreach($domBindings as $domBinding) {
			$nodes = $domBinding->children();

			foreach ($nodes as $node) {
				$domRules = $node->children();

				foreach($domRules as $domRule) {
					$domRule['type'] = $node->getName();
					$this->bindingsQueue->add($domRule);
				}
			}
		}

		$this->bindingsQueue->sort();
	}

	function setModules($module) {
		$this->modules[$module->getName()] = $module; 
	}

	/**
	 * Преобразует 'all', 'selected', 'none' в числовые значения.
	 *
	 * @see PP_CHILDREN_FETCH_ALL, PP_CHILDREN_FETCH_SELECTED, PP_CHILDREN_FETCH_NONE
	 * @access protected
	 * @param String $string
	 * @return Integer
	 */

	/* FIXME - this is private, check external usage!!! i.e. objectform.class.inc:266 */
	public static function _mapByDefaults($string) {
		if ($string == 'all') {
			return PP_CHILDREN_FETCH_ALL;
		}
		if ($string == 'selected') {
			return PP_CHILDREN_FETCH_SELECTED;
		}
		if ($string == 'none') {
			return PP_CHILDREN_FETCH_NONE;
		}
		return NULL;
	}

	/**
	 * Проверяет наличие кешированного файла настроек и загружает его.
	 *
	 * Если существует актуальный tmp/cache/config файл то он десериализуется в this.
	 * Актуальность проверяется по времени последнего изменения каталога с конфиг файлами.
	 *
	 * @param  String   $projectDir каталог с файлами
	 * @return Boolean  true если файл существует
	 */
	private static function loadFromCache($projectDir) {
		$cacheFile = BASEPATH.DIRECTORY_SEPARATOR.'tmp'.DIRECTORY_SEPARATOR.'cache'.DIRECTORY_SEPARATOR.'config_'.md5($projectDir);

		if (!file_exists($cacheFile)) {
			return null;
		}

		$tmp = stat($cacheFile);
		$cacheMtime = $tmp[9];

		foreach(self::configDirs($projectDir) as $configDir) {
			$d = new NLDir($configDir);

			while ($entry = $d->ReadFull()) {
				$tmp = stat($entry);

				if ($tmp[9] >= $cacheMtime) {
					return null;
				}
			}
		}

		return unserialize(ReadFileToString($cacheFile));
	}

	/**
	 * Кеширует настройки PXApplication в файл.
	 *
	 * Весь PXApplication сериализуется в tmp/cache/config файл.
	 */
	private function saveToCache($projectDir) {
		$cacheFile = BASEPATH.DIRECTORY_SEPARATOR.'tmp'.DIRECTORY_SEPARATOR.'cache'.DIRECTORY_SEPARATOR.'config_'.md5($projectDir);

		MakeDirIfnotExists(dirname($cacheFile));
		WriteStringToFile($cacheFile, serialize($this));
	}
}

?>