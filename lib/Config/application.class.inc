<?php
require_once PPLIBPATH . 'Config/Description/db.class.inc';

require_once PPLIBPATH . 'Config/Description/type.class.inc';
require_once PPLIBPATH . 'Config/Description/field.class.inc';

require_once PPLIBPATH . 'Config/Description/directory.class.inc';
require_once PPLIBPATH . 'Config/Description/ref.class.inc';

require_once PPLIBPATH . 'Config/Description/module.class.inc';
require_once PPLIBPATH . 'Config/bindingsQueue.class.inc';

require_once PPLIBPATH . 'Config/Description/trigger.class.inc';
require_once PPLIBPATH . 'Config/triggersList.class.inc';

require_once PPLIBPATH . 'Config/Description/plugin.class.inc';

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
	 * контейнер c триггерами {@link PXTriggersList}
	 * @var Object
	 */
	var $triggers = null;

	/**
	 * массив модулей {@link PXModuleDescription}
	 * @var Array
	 */
	var $modules;

	/**
	 * массив плагинов
	 * @var Array
	 */
	var $plugins = null;

	/**
	 * массив настроек из etc/properties.ini
	 * @var Array
	 */
	var $properties;

	/**
	 * массив языковых настроек из (site|local|libpp)/etc/lang.yaml (суммарный)
	 * @var Array
	 */
	var $langTree = array();

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
	 * экземпляр класса Engine
	 * @var Object
	 */
	public $engine;

	/**
	 * массив названий полей, чьи объекты требуют инициализации при загрузке из кэша.
	 * Description объекта должен реализовывать метод initialize
	 * @var Array
	 **/
	protected $reinitialize = array();

	/**
	 * Singleton array with caching.
	 *
	 */
	public static function getInstance($engineObject) {
		static $instance = array();

		$engineClass = get_class($engineObject);

		if (isset($instance[$engineClass])) {
			return $instance[$engineClass];
		}

		/* try to get config from cache */
		$cache = self::loadFromCache($engineClass);

		if(!is_null($cache)) {
			$instance[$engineClass] = $cache;
			return $instance[$engineClass];
		}

		/* create new instance, if not cached */
		if (!isset($instance[$engineClass])) {
			$instance[$engineClass] = new PXApplication($engineClass, $engineObject);
		}

		return $instance[$engineClass];
	}

	/**
	 * Конструктор класса
	 *
	 * @param String $engineClass - имя класса Engine, используется в имени файла кэша
	 */
	private function __construct($engineClass, $engineObject) {
		$this->engine = $engineObject;
		$this->fillDBDescription(self::parseIniFile($this->findConfigFile('database.ini')));
		$this->fillTriggers($this->findConfigFile('triggers.xml', false));

		$this->fillPlugins($this->findConfigFile('plugins.xml', false));

		$this->fillModules($this->findConfigFile('modules.xml'));
		$this->fillDataTypes($this->findConfigFile('datatypes.xml'));
		$this->fillDirectories($this->findConfigFile('datatypes.xml'));

		$this->properties = self::parseIniFiles($this->findConfigFiles('properties.ini'));
		$this->authrules  = self::parseIniFiles($this->findConfigFiles('auth.ini'));

		$this->fillLang($this->findConfigFiles('lang.yaml'));

		$this->saveToCache($engineClass);
	}

	private function fillLang ($filenames) {
		// use precompiled lang.yaml if exists
		if (file_exists(SHAREDPATH . 'var/lang.yaml')) {
			array_unshift($filenames, SHAREDPATH . 'var/lang.yaml'); // prod path
		} elseif (file_exists(BASEPATH . 'tmp/lang.yaml')) {
			array_unshift($filenames, BASEPATH . 'tmp/lang.yaml'); // dev path
		}

		foreach (array_reverse($filenames) as $filename) {
			$source = ReadFileToString($filename);
			$this->langTree = array_recursive_inheritance($this->langTree, PXYaml::parse($source));
		}
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
	 * @return Array         массив из элементов 'module_name' => $module
	 */
	public function getAvailableModules() {
		return array_filter($this->modules, function($module) { return PXRegistry::getUser()->can('admin', $module); });
	}

	/**
	 * Возвращает модуль по его имени с проверкой доступа к нему.
	 *
	 * @return PXModule модуль
	 */
	public function getAvailableModule($module_name) {
		return isset($this->modules[$module_name]) && PXRegistry::getUser()->can('admin', $this->modules[$module_name]) ? $this->modules[$module_name] : null;
	}

	/**
	 * Возвращает ассоциативный массив доступных модулей.
	 *
	 * @param String $package путь пакаджа (например, "csv/import")
	 * @return Array          массив из элементов 'module_name' => $module
	 */
	public function getAvailableModulesWithinPackage($package) {
		$availableModules = $this->getAvailableModules();
		if (empty($package)) {
			return $availableModules;
		}

		return array_filter($availableModules, function($module) use ($package) { return 0 === strpos($module->package, $package); });
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
	 *  @uses getAllowedChilds
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

	public function initObject($formatName) {
		return $this->initContentObject($formatName);
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
		if (!($dom = PXml::load($fileName))) {
			FatalError("Ошибка в файле настроек <EM>$fileName</EM>");
		}

		PXTypeDescription::fillAppTypes($dom->xpath("/model/datatypes/datatype"), $this);
		PXRefDescription::fillReferences($dom->xpath("/model/references/reference"), $this);
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
		if (!($dom = PXml::load($fileName))) {
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

			$sourcekeyfield = trim($domDirectory->getAttribute('source-key-field'));
			$curDir->sourceKeyField = strlen($sourcekeyfield) ? $sourcekeyfield : '@id';

			$sourcedisplayfield = trim($domDirectory->getAttribute('source-display-field'));
			$curDir->sourceDisplayField = strlen($sourcedisplayfield) ? $sourcedisplayfield : '.';

			$sourceitem = trim($domDirectory->getAttribute('source-item'));
			$curDir->sourceItem = strlen($sourceitem) ? $sourceitem : './*';

			$sourcedatafields = trim($domDirectory->getAttribute('source-data-fields'));
			$curDir->sourceDataFields = strlen($sourcedatafields) ? $sourcedatafields : '@*[local-name() != "id"]';

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

		$idField       = $directory->sourceKeyField;
		$titleField    = $directory->sourceDisplayField;
		$xitem         = $directory->sourceItem;
		$dataFields    = $directory->sourceDataFields;

		if (!($dom2 = PXml::load($file))) {
			FatalError("Ошибка в файле справочника <EM>$file</EM>");
		}

		$results = $dom2->xpath($xpathSelector);

		if ($results) {

			foreach ($results as $result) {

				$nodes = $result->xpath($xitem);
				if (empty($nodes)) {
					continue;
				}

				foreach ($nodes as $node) {
					$id = $node->nodeXValue($idField);

					$directory->values[$id] = array(
						'id'    => $id,
						'title' => pp_simplexml_decode_string($node->nodeXValue($titleField))
					);

					$data = $node->xpath($dataFields);

					// Structure attributes fix
					foreach ($data as $dataNode) {
						$key = $dataNode->nodeName();
						if ($dataNode->nodeType() == PXml::ATTRIBUTE) {
							$directory->values[$id][$key] = pp_simplexml_decode_string($dataNode->nodeValue());
							continue;
						}

						$item = array();
						foreach ($dataNode->attributes() as $val) {
							$item[$val->name] = pp_simplexml_decode_string($val->value);
						}
						foreach ($dataNode->childNodes() as $val) {
							$item[$val->nodeName()] = pp_simplexml_decode_string($val->nodeValue());
						}

						empty($directory->values[$id][$key]) && ($directory->values[$id][$key] = array());
						$directory->values[$id][$key][] = $item;
					}
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
	private function fillModules($fileName) {
		if (!($dom = PXml::load($fileName))) {
			FatalError("Ошибка в файле настроек <em>$fileName</em>");
		}

		$domModules = $dom->xpath("/modules//module");

		if (!is_array($domModules) || count($domModules) == 0) {
			FatalError("Не определены модули проекта");
		}

		foreach ($domModules as $domModule) {
			$module = new PXModuleDescription();
			$module->setAttrs($domModule->attributes());
			$module->lookPackage($dom);
			$module->checkClass();

			$nodes = $domModule->childNodes();
			foreach ($nodes as $node) {
				if (!$node->isXmlNode()) continue;

				if (!empty($node->childNodes())) {
					$settings = [];
					foreach ($node->childNodes() as $child) {
						$chunk = [];
						foreach ($child->attributes() as $attr) {
							$chunk[$attr->name] = $attr->value;
						}
						$settings[] = $chunk;
					}
					$module->settings[$node->nodeName()] = $settings;
				} else {
					$module->settings[$node->nodeName()][] = pp_simplexml_decode_string($node->nodeValue());
				}
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
	 * Пользовательская функция для регистрации модулей не через конфиг modules.xml, а при загрузке плагинов
	 * @param  String   $name - имя
	 * @param  Array    $args - массив с параметрами триггера, имя => значение
	 */
	public function registerModule ($name, array $args = null) {
		if (!ctype_alnum($name)) {
			throw new Exception('Invalid module name');
		}

		$lcname = strtolower($name);
		$ucfname = ucfirst($name);

		if (isset($this->modules[$lcname])) {
			return false;
		}

		$module = new PXModuleDescription();
		$module->name        = $lcname;
		$module->description = isset($args['description'])? $args['description'] : $ucfname;
		$module->class       = isset($args['class'])? $args['class'] : 'PXModule' . $ucfname;
		$this->setModules($module);

		return true;
	}

	/*
	 * plugins
	 *
	 */
	function fillPlugins($fileName) {
		if($fileName === false) {
			return;
		}

		if (!($dom = PXml::load($fileName))) {
			FatalError("Ошибка в файле настроек <em>$fileName</em>");
		}

		$domPlugins = $dom->xpath("/plugins//plugin");

		if (!is_array($domPlugins) || count($domPlugins) == 0) {
			FatalError("Не описаны плагины проекта");
		}

		foreach($domPlugins as $domPlugin) {
			$plugin = new PXPluginDescription();
			$plugin->setAttrs($domPlugin->attributes());

			if ($domPlugin->param) {
				$plugin->setParams($domPlugin->param);
			}

			if ($domPlugin->modules) {
				$plugin->setModules($domPlugin->modules);
			}

			if ($domPlugin->triggers) {
				$plugin->setTriggers($domPlugin->triggers);
			}

			$this->plugins[$plugin->getName()] = $plugin;
			$plugin->getPlugin($this); //initialize plugin
		}

		$this->reinitialize['plugins'] = true;
	}

	/**
	 * Заполняет {@link PXApplication::$triggers} данными из файла.
	 *
	 * В качестве параметра обычно используется файл etc/triggers.xml. <br>
	 * XPath /triggers//trigger
	 * Инициализирует поле triggers объектом-коллекцией триггеров
	 *
	 * @uses PXTriggerDescription
	 *
	 * @param String $fileName имя файла
	 */
	private function fillTriggers($fileName) {
		$this->triggers = new PXTriggersList;

		if($fileName === false) {
			return;
		}

		if (!($dom = PXml::load($fileName))) {
			FatalError("Ошибка в файле настроек <em>$fileName</em>");
		}

		$domTriggers = $dom->xpath("/triggers//trigger");

		if (!is_array($domTriggers) || count($domTriggers) == 0) {
			FatalError("Не описаны триггеры проекта");
		}

		foreach ($domTriggers as $domTrigger) {
			$this->setTrigger($domTrigger->attributes(), $domTrigger->parent()->nodeName());
		}
	}

	private function setTrigger($attributes, $type) {
		$trigger = new PXTriggerDescription();
		$trigger->setAttrs($attributes);
		$trigger->setType($type);

		if(!isset($this->triggers[$type])) {
			$this->triggers[$type] = array();
		}

		$this->triggers[$type][$trigger->getName()] = $trigger;
	}

	/**
	 * Пользовательская функция для регистрации триггеров не через конфиг triggers.xml, а в процессе работы системы
	 * @param  String   $type - тип триггера
	 * @param  Array    $args - массив с параметрами триггера, имя => значение
	 */
	function registerTrigger($type, array $args){
		foreach($args as $name => &$value){
			$value = PXml::attributePrototype($name, $value); //XML style attributes compatibility
		}

		$this->setTrigger($args, $type);
		return $this;
	}

	/**
	 * Проверяет наличие кешированного файла настроек и загружает его.
	 *
	 * Если существует актуальный tmp/cache/config файл то он десериализуется в this.
	 * Актуальность проверяется по времени последнего изменения каталога с конфиг файлами.
	 *
	 * @param  String   $engineClass каталог с файлами
	 * @return String   NULL если файл не существует
	 */
	private static function loadFromCache($engineClass) {
		$cacheFile = self::_cacheFilename($engineClass);

		if (!file_exists($cacheFile)) {
			return null;
		}

		$tmp        = stat($cacheFile);
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

		$app = unserialize(ReadFileToString($cacheFile));
		return $app;
	}

	/**
	 * Кеширует настройки PXApplication в файл.
	 *
	 * Весь PXApplication сериализуется в tmp/cache/config файл.
	 */
	private function saveToCache($engineClass) {
		$cacheFile = self::_cacheFilename($engineClass);

		MakeDirIfNotExists(dirname($cacheFile));
		WriteStringToFile($cacheFile, serialize($this));
	}

	private static function _cacheFilename($name){
		return CACHE_PATH.'/config/'.strtolower($name);
	}

	private static function parseIniFile($file) {
		return (array) parse_ini_file($file, true);
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
	 * function is_enough($files_array) { <return TRUE when enough files collected> }
	 * @param string $filename
	 * @param callable $is_enough
	 */
	private static function findFilesUnless($filename, $is_enough = NULL) {
		$files = array();

		foreach(self::configDirs() as $configDir) {
			$path = $configDir . $filename;

			if (file_exists($path)) {
				$files[] = $path;
			}

			if (is_callable($is_enough) && $is_enough($files)) {
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

	/**
	 * Returns application path
	 */
	public function getAppPath() {
		return APPPATH;
	}

	/**
	 * Return if application is in development mode
	 * @return bool
	 */
	public function isDevelopmentMode() {
		return $this->getProperty('ENVIRONMENT') === ENV_DEVELOPER;
	}

	function isAdminEngine() {
		return $this->engine->engineClass() == PX_ENGINE_ADMIN;
	}

	function isEngineInstanceof($engineName) {
		$engineName = 'PxEngine' . $engineName;
		return $this->engine instanceof $engineName;
	}

	function __wakeup() {
		foreach($this->reinitialize as $name=>$bool) {
			foreach((array) $this->{$name} as $o) {
				$o->initialize($this);
			}
		}
	}
}
