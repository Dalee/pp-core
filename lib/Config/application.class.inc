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
	var $types = array();

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
	var $directory = array();

	/**
	 * биндинги модулей из etc/modules.xml modules/bindings
	 * @var Array
	 */
	var $bindingsQueue;

	/**
	 * Singleton array with caching.
	 *
	 */
	public static function getInstance($engineClass) {
		static $instance = array();

		if (isset($instance[$engineClass])) {
			return $instance[$engineClass];
		}

		/* try to get config from cache */
		$cache = self::loadFromCache($engineClass);

		if(!is_null($cache))
			return $instance[$engineClass] = $cache;

		/* create new instance, if not cached */
		if (!isset($instance[$engineClass])) {
			$instance[$engineClass] = new PXApplication($engineClass);
		}

		return $instance[$engineClass];
	}

	/**
	 * Конструктор класса
	 *
	 * @param String $engineClass - имя класса Engine, используется в имени файла кэша
	 */
	private function __construct($engineClass) {
		$this->engineClass = $engineClass;

		$this->FillDBDescription(self::parseIniFile($this->findConfigFile('database.ini')));

		$this->FillModules($this->findConfigFile('modules.xml'));
		$this->FillDataTypes($this->findConfigFile('datatypes.xml'));
		$this->FillDirectories($this->findConfigFile('datatypes.xml'));

		$this->properties = $this->parseProperties();
		$this->authrules  = self::parseIniFiles($this->findConfigFiles('auth.ini'));

		//todo: one file or many lang files?
		$this->fillLang($this->findConfigFile('lang.yaml'));

		$this->saveToCache($engineClass);
	}

	private static function parseIniFile($file) {
		return (array) parse_ini_file($file, true);
	}

	private static function parseIniFiles($files) {
		/* we must merge array in reverse order: libpp + local + site,
		 * so local can redefine libpp settings, and site can override local */

		$result = self::parseIniFile(array_pop($files));

		while(count($files)) {
			$result = $result + self::parseIniFile(array_pop($files));

			/*foreach($tmp as $k => $v) {
				if (is_array($v) and isset($result[$k]) and is_array($result[$k])) {
					$result[$k] = array_merge($result[$k], $v);
				} else {
					$result[$k] = $v;
				}
			}*/
		}

		return $result;
	}

	/**
	 * @return Array
	 */
	function parseProperties() {
		list($override, $default) = array(array(), array());

		$default_file_exists = file_exists($default_file  = self::findConfigFile('default/properties.ini', false));
		$user_file_exists    = file_exists($override_file = self::findConfigFile('properties.ini'));

		//пропарсит дефолтный файл если он существует
		if($default_file_exists) {
			$default = self::ParseIniFile($default_file);
		}

		//если есть дефолтный файл но нет юзерского то возращает значение из
		if(!$user_file_exists && $default_file_exists) {
			return $default;
		}

		// парсинг юзерского файла
		// возбуждение ошибки в случае если нет файла
		$override = self::ParseIniFile($override_file);

		//возращает склеяный результат
		return $override + $default;
	}


	/**
	 *  Searches for the first file in order - site, local, libpp
	 */
	private static function findConfigFile($filename, $required=true) {
		$files = self::findFilesUnless($filename,
			function($files) { return count($files) > 0; });

		if ($files) {
			return $files[0];
		}

		if($required) {
			FatalError('Отсутствует файл настроек ' . $filename);

		} else {
			return false;
		}
	}

	/**
	 *  Searches for all files at site, local, libpp dirs.
	 */
	private static function findConfigFiles($filename) {
		if ($files = self::findFilesUnless($filename)) {
			return $files;
		}

		FatalError('Отсутствует файл настроек '.$filename);
	}

	/*
	 * function is_enought($files_array) { <return TRUE when enought files collected> }
	 */
	private static function findFilesUnless($filename, $is_enough = NULL) {
		$files = array();

		foreach(self::configDirs() as $configDir) {
			$path = $configDir . $filename;

			if (file_exists($path)) {
				$files[] = $path;
			}

			if ($is_enough && $is_enough($files)) {
				return $files;
			}
		}

		return $files;
	}

	private static function configDirs() {
		return array(
			BASEPATH.'/site/etc/',
			BASEPATH.'/local/etc/',
			BASEPATH.'/libpp/etc/');
	}

	//TODO:FIXME: REWRITE ME!
	private function fillLang ($filename) {
		if (!function_exists("__ReplaceIncludeWhenParseLangYaml")) {
			function __ReplaceIncludeWhenParseLangYaml($replacement, $indent, $fileName) {
				$string = '';

				if(file_exists($fileName)) {
					$tmp = @file($fileName);

				} elseif(file_exists(BASEPATH.$fileName)) {
					$tmp = @file(BASEPATH.$fileName);

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
		}
		
		$source = ReadFileToString($filename);

		$source = preg_replace('|^((\s*)include\s+(.+?))$|ime', "__ReplaceIncludeWhenParseLangYaml('$1', '$2', '$3');", $source);   // делаем include
		$source = preg_replace('/\t/',          ' ' ,         $source);   // я люблю табы для читабельности
		$source = preg_replace('/(\r?\n){2,}/', "\n",         $source);   // и лишние переводы строк

		$tmp = tempnam(BASEPATH.'tmp/lang/', 'lang');
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
	 * @param Integer $leve  название текущего раздела
	 * @return Array         массив из элементов 'module_name' => $module
	 */
	public function getAvailableModules($level) {
		return array_filter($this->modules, create_function('$module', "return \$module->access <= $level;"));
	}

	/**
	 * Возвращает ассоциативный массив разрешенных детей.
	 *
	 * @param NULL $formatName      не используется
	 * @param Object|Array $object  объект для которого искать детей
	 * @return Array                массив разрешенных детей или пустой массив
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
		$dom = PXML::load($fileName);
		if (!$dom) {
			FatalError("Ошибка в файле настроек <EM>$fileName</EM>");
		}

		PXTypeDescription::fillAppTypes($dom, $this);
		PXRefDescription::fillReferences($dom, $this);
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
		$dom = PXML::load($fileName);

		if (!$dom) {
			FatalError("Ошибка в файле настроек <EM>$fileName</EM>");
		}
		$domDirectories = $dom->xpath("/model/directories/directory");

		foreach ($domDirectories as $domDirectory) {
			$name = $domDirectory->getAttribute('name');

			if (!isset($this->directory[$name])) {
				$this->directory[$name] = new PXDirectoryDescription($name);
			}

			$curDir =& $this->directory[$name];

			$curDir->load         = $domDirectory->getAttribute('load');
			$curDir->schema       = $domDirectory->getAttribute('schema');
			$curDir->source       = $domDirectory->getAttribute('source');
			$curDir->sourceDir    = dirname($fileName).DIRECTORY_SEPARATOR;

			$displayfield = trim($domDirectory->getAttribute('displayfield'));
			$curDir->displayField = strlen($displayfield) ? $displayfield : 'title';

			$location = $domDirectory->getAttribute('location');
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

		$dom2 = PXML::load($file);

		if (!$dom2){
			FatalError("Ошибка в файле справочника <EM>$file</EM>");
		}
		$results = $dom2->xpath($xpathSelector);

		if ($results) {
			$nodes = $results[0]->childNodes();

			foreach ($nodes as $node) {
				$id = $node->getAttribute('id');

				$directory->values[$id] = array(
					'id'    => $id,
					'title' => utf8_decode($node->nodeValue()) 
				);

				$attrs = $node->attributes();

				// Structure attributes fix
				foreach($attrs as $attr_val){
					if ($attr_val->name == 'id') {
						continue;
					}

					$directory->values[$id][$attr_val->name] = utf8_decode($attr_val->value);
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
		$dom = PXML::load($fileName);

		if (!$dom) {
			FatalError("Ошибка в файле настроек <em>$fileName</em>");
		}

		$domModules = $dom->xpath("/modules//module");

		if (!$domModules || empty($domModules)) {
			FatalError("Не определены модули проекта");
		}

		foreach($domModules as $domModule) {
			$module = new PXModuleDescription();
			$module->setAttrs($domModule->attributes());
			$module->lookPackage($dom);

			$nodes = $domModule->childNodes();
			foreach ($nodes as $node) {
				$module->settings[$node->nodeName()][] = utf8_decode($node->nodeValue());
			}

			foreach ($module->settings as $k=>$v) {
				if (count($v) == 1) {
					$module->settings[$k] = $v[0];
				}
			}

			$this->setModules($module);
		}


		$domBindings = $dom->xpath('/modules/bindings');
		$this->bindingsQueue = new PXBindingsQueue();

		foreach($domBindings as $domBinding) {
			$nodes = $domBinding->childNodes();

			foreach ($nodes as $node) {
				$domRules = $node->childNodes();

				foreach($domRules as $domRule) {
					$this->bindingsQueue->add($domRule, $node->nodeName());
				}
			}
		}

		$this->bindingsQueue->sort();
	}

	function setModules($module) {
		$this->modules[$module->getName()] = $module; 
	}

	/**
	 * Проверяет наличие кешированного файла настроек и загружает его.
	 *
	 * Если существует актуальный tmp/cache/config файл то он десериализуется в this.
	 * Актуальность проверяется по времени последнего изменения каталога с конфиг файлами.
	 *
	 * @param  String   $engineClass каталог с файлами
	 * @return Boolean  true если файл существует
	 */
	private static function loadFromCache($engineClass) {
		$cacheFile = self::cacheFilename($engineClass);

		if (!file_exists($cacheFile)) {
			return null;
		}

		$tmp = stat($cacheFile);
		$cacheMtime = $tmp['mtime'];

		foreach(self::configDirs() as $configDir) {
			$d = new NLDir($configDir);

			while ($entry = $d->ReadFull()) {
				$tmp = stat($entry);

				if ($tmp['mtime'] >= $cacheMtime) {
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
	private function saveToCache($engineClass) {
		$cacheFile = self::cacheFilename($engineClass);

		MakeDirIfnotExists(dirname($cacheFile));
		WriteStringToFile($cacheFile, serialize($this));
	}

	private static function cacheFilename($name){
		return BASEPATH.'/tmp/cache/config/'.strtolower($name);
	}
}

?>
