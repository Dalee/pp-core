<?php

use Symfony\Component\Yaml\Yaml;
use PP\Lib\ArrayCollection;
use PP\Lib\Config\Description\InitializableDescriptionInterface;
use PP\Lib\Xml\Xml;
use PP\Properties\PropertyLoader;
use PP\Module\AbstractModule;
use PP\Properties\EnvLoader;

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
	 * @var NLDBDescription[]
	 */
	var $dbDescription;

	/**
	 * массив описаний datatypes {@link PXTypeDescription}
	 * @var PXTypeDescription[]
	 */
	var $types = [];

	/**
	 * массив ссылок между datatypes {@link PXRefDescription}
	 * @var PXRefDescription[]
	 */
	var $references;

	/**
	 * контейнер c триггерами {@link PXTriggersList}
	 * @var PXTriggersList[]
	 */
	var $triggers = null;

	/**
	 * массив модулей {@link PXModuleDescription}
	 * @var PXModuleDescription[]
	 */
	var $modules;

	/**
	 * массив плагинов
	 * @var PXPluginDescription[]
	 */
	var $plugins = null;

	/**
	 * Объект настроек
	 *
	 * @var ArrayCollection
	 */
	protected $properties;

	/**
	 * TODO: refactor to protected
	 * массив языковых настроек из (site|local|libpp)/etc/lang.yaml (суммарный)
	 *
	 * @var ArrayCollection
	 */
	public $langTree;

	/**
	 * массив правил авторизации из etc/auth.ini
	 * @var array
	 */
	var $authrules = [];

	/**
	 * массив описаний каталогов из etc/datatypes.xml
	 * @var array
	 */
	var $directory = [];

	/**
	 * биндинги модулей из etc/modules.xml modules/bindings
	 * @var PXBindingDescription[]
	 */
	var $bindingsQueue;

	/**
	 * экземпляр класса Engine
	 * @var \PP\Lib\Engine\AbstractEngine
	 */
	public $engine;

	/**
	 * массив названий полей, чьи объекты требуют инициализации при загрузке из кэша.
	 * Description объекта должен реализовывать метод initialize
	 *
	 * @var array
	 **/
	protected $reinitialize = [];

	/**
	 * Singleton array with caching.
	 * @param \PP\Lib\Engine\AbstractEngine $engineObject
	 * @return $this
	 */
	public static function getInstance($engineObject) {
		/** @var PXApplication[] $instance */
		static $instance = [];

		$engineClass = get_class($engineObject);
		$instance[$engineClass] = self::loadFromCache($engineClass);

		if (!empty($instance[$engineClass])) {
			return $instance[$engineClass];
		}

		if (empty($instance[$engineClass])) {
			$instance[$engineClass] = new PXApplication(
				$engineClass,
				$engineObject
			);
		}

		return $instance[$engineClass];
	}

	/**
	 * Конструктор класса
	 *
	 * @param string $engineClass - имя класса Engine, используется в имени файла кэша
	 * @param \PP\Lib\Engine\AbstractEngine $engineObject
	 */
	private function __construct($engineClass, $engineObject) {
		EnvLoader::inject();
		$this->langTree = new ArrayCollection();
		$this->properties = new ArrayCollection(); // properties will be filled via call in PXAbstractEngine
		$this->engine = $engineObject;

		$this->fillDbDescription();
		$this->fillTriggers($this->findConfigFile('triggers.xml', false));
		$this->fillPlugins($this->findConfigFile('plugins.xml', false));
		$this->fillModules(APPPATH . join(DIRECTORY_SEPARATOR, ['config', 'modules.yml']));
		$this->fillDataTypes($this->findConfigFile('datatypes.xml'));
		$this->fillDirectories($this->findConfigFile('datatypes.xml'));
		$this->fillLang($this->findConfigFiles('lang.yaml'));
		$this->fillAuthRules($this->findConfigFiles('auth.ini'));

		// save constructed app to cache
		$this->saveToCache($engineClass);
	}

	/**
	 * Load and merge different yaml files
	 *
	 * @param string[] $fileList
	 */
	private function fillLang($fileList) {
		$fileList = array_reverse($fileList);
		$langList = array_map(function ($file) {
			return Yaml::parse(file_get_contents($file));
		}, $fileList);

		foreach ($langList as $data) {
			$this->appendLang($data);
		}
	}

	/**
	 * @param array $langArray lang data array to be merged into base one
	 */
	public function appendLang($langArray) {
		$langArrayCurrent = $this->langTree->toArray();
		$langArrayCurrent = array_recursive_inheritance($langArrayCurrent, $langArray);
		$this->langTree->fromArray($langArrayCurrent);
	}

	/**
	 * Создает описание подключения к базе данных
	 * @see NLDBDescription, $dbDescription
	 *
	 */
	private function fillDbDescription() {
		$this->dbDescription = [
			'database' => NLDBDescription::fromEnv()
		];
	}

	/**
	 * @param string[] $files
	 */
	private function fillAuthRules($files) {
		$this->authrules = self::parseIniFiles($files);
	}

	/**
	 * Возвращает элемент из ({@link properties}) по ключу или значение по умолчанию.
	 *
	 * @param string $k ключ
	 * @param string $defaultValue - возвращаемое значение по умолчанию
	 * @return mixed
	 */
	public function getProperty($k, $defaultValue = null) {
		return (isset($this->properties[$k])) ? $this->properties[$k] : $defaultValue;
	}

	/**
	 * Возвращает ассоциативный массив доступных модулей.
	 *
	 * @return array         массив из элементов 'module_name' => $module
	 */
	public function getAvailableModules() {
		return array_filter($this->modules, function ($module) {
			return PXRegistry::getUser()->can('admin', $module);
		});
	}

	/**
	 * Возвращает модуль по его имени с проверкой доступа к нему.
	 *
	 * @return AbstractModule модуль
	 */
	public function getAvailableModule($module_name) {
		return isset($this->modules[$module_name]) && PXRegistry::getUser()->can('admin', $this->modules[$module_name]) ? $this->modules[$module_name] : null;
	}

	/**
	 * Возвращает ассоциативный массив доступных модулей.
	 *
	 * @param String $package путь пакаджа (например, "csv/import")
	 * @return array          массив из элементов 'module_name' => $module
	 */
	public function getAvailableModulesWithinPackage($package) {
		$availableModules = $this->getAvailableModules();
		if (empty($package)) {
			return $availableModules;
		}

		return array_filter($availableModules, function ($module) use ($package) {
			return 0 === strpos($module->package, $package);
		});
	}

	/**
	 * Возвращает ассоциативный массив разрешенных детей.
	 *
	 * @param NULL $formatName не используется
	 * @param Object|array $object объект для которого искать детей
	 * @return array                массив разрешенных детей или пустой массив
	 */
	public function getAllowedChilds($formatName, &$object) {
		if (!isset($this->types[$formatName])) {
			return array();
		}

		return $this->types[$formatName]->allowedChilds($object);
	}

	/**
	 * Возвращает массив наименований разрешенных форматов дочерних объектов.
	 *
	 * @uses getAllowedChilds
	 *
	 * @param string $formatName не используется
	 * @param array $object объект для которого искать детей
	 * @return array        массив наименований разрешенных детей или пустой массив
	 */
	public function getAllowedChildsKeys($formatName, &$object) {
		return array_keys($this->getAllowedChilds($formatName, $object));
	}

	/**
	 * @param string $formatName
	 * @return array
	 */
	public function initContentObject($formatName) {
		$object = array();

		foreach ($this->types[$formatName]->fields as $k => $v) {
			$object[$k] = $v->defaultValue;
		}

		return $object;
	}

	/**
	 * @deprecated will be removed
	 * @param $formatName
	 * @return array
	 */
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
	 * @param String $fileName имя файла
	 */
	private function fillDataTypes($fileName) {
		if (!($dom = Xml::load($fileName))) {
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
	 * @param String $fileName имя файла
	 */
	private function fillDirectories($fileName) {
		if (!($dom = Xml::load($fileName))) {
			FatalError("Ошибка в файле настроек <EM>$fileName</EM>");
		}

		$domDirectories = $dom->xpath("/model/directories/directory");

		foreach ($domDirectories as $domDirectory) {
			$name = $domDirectory->getAttribute('name');

			if (!isset($this->directory[$name])) {
				$this->directory[$name] = new PXDirectoryDescription($name);
			}

			$curDir =& $this->directory[$name];

			$curDir->load = $domDirectory->getAttribute('load');
			$curDir->schema = $domDirectory->getAttribute('schema');
			$curDir->source = $domDirectory->getAttribute('source');
			$curDir->sourceDir = dirname($fileName) . DIRECTORY_SEPARATOR;

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
	 * Load properties from database.
	 *
	 * @param PXDatabase $database
	 */
	public function loadProperties(PXDatabase $database) {
		$propertyList = PropertyLoader::getPropertyList($database);
		$this->properties->fromArray($propertyList);
	}

	/**
	 * Loads a directory by link.
	 * Uses {@link PXDatabase::_GetData()} to load directories from database.
	 *
	 * @param PXDirectoryDescription $directory
	 * @param bool $addDefault
	 */
	public function LoadXMLDirectory(&$directory, $addDefault = true) {
		$file = self::findConfigFile($directory->source);
		$xpathSelector = $directory->location;

		$idField = $directory->sourceKeyField;
		$titleField = $directory->sourceDisplayField;
		$xitem = $directory->sourceItem;
		$dataFields = $directory->sourceDataFields;

		if (!($dom2 = Xml::load($file))) {
			FatalError("Ошибка в файле справочника <EM>$file</EM>");
		}

		$results = $dom2->xpath($xpathSelector);

		if ($results) {

			/** @var \PP\Lib\Xml\XmlNodeInterface $result */
			foreach ($results as $result) {

				$nodes = $result->xpath($xitem);
				if (empty($nodes)) {
					continue;
				}

				/** @var \PP\Lib\Xml\XmlNodeInterface $node */
				foreach ($nodes as $node) {
					$id = $node->nodeXValue($idField);

					$directory->values[$id] = array(
						'id' => $id,
						'title' => pp_simplexml_decode_string($node->nodeXValue($titleField)),
					);

					$data = $node->xpath($dataFields);

					// Structure attributes fix
					foreach ($data as $dataNode) {
						$key = $dataNode->nodeName();
						if ($dataNode->nodeType() == Xml::ATTRIBUTE) {
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

	private function fillModules($fileName) {
		$modules = Yaml::parse(file_get_contents($fileName));

		$modulesList = getFromArray($modules, 'modules', []);
		if (!is_array($modulesList) || count($modulesList) == 0) {
			FatalError('Не определены модули проекта');
		}

		foreach ($modulesList as $moduleName => $module) {
			$description = (new PXModuleDescription())
				->setName($moduleName)
				->setDescription($module['description'])
				->setClass($module['class'])
				->setSettings(getFromArray($module, 'params', []));

			$this->setModules($description);
		}

		$this->bindingsQueue = new PXBindingsQueue();

		$modulesBindings = getFromArray($modules, 'bindings', []);
		foreach ($modulesBindings as $bindingType => $bindings) {
			foreach ($bindings as $binding) {
				$this->bindingsQueue->add($binding, $bindingType);
			}
		}

		$this->bindingsQueue->sort();
	}

	/**
	 * @param PXModuleDescription $module
	 */
	function setModules(PXModuleDescription $module) {
		$this->modules[$module->getName()] = $module;
	}

	/**
	 * Пользовательская функция для регистрации модулей не через конфиг modules.yml, а при загрузке плагинов
	 * @param  string $name - имя
	 * @param  array $args - массив с параметрами триггера, имя => значение
	 * @return bool
	 * @throws \Exception
	 */
	public function registerModule($name, array $args = []) {
		if (!ctype_alnum($name)) {
			throw new Exception('Invalid module name');
		}

		$lcname = strtolower($name);
		$ucfname = ucfirst($name);

		if (isset($this->modules[$lcname])) {
			return false;
		}

		$module = (new PXModuleDescription())
			->setName($lcname)
			->setDescription(isset($args['description']) ? $args['description'] : $ucfname)
			->setClass(isset($args['class']) ? $args['class'] : 'PXModule' . $ucfname);

		$this->setModules($module);

		return true;
	}

	/**
	 * Load plugins.
	 *
	 * @param string $fileName
	 */
	private function fillPlugins($fileName) {
		if ($fileName === false) {
			return;
		}

		if (!($dom = Xml::load($fileName))) {
			FatalError("Ошибка в файле настроек <em>$fileName</em>");
		}

		$domPlugins = $dom->xpath("/plugins//plugin");
		if (!is_array($domPlugins) || count($domPlugins) == 0) {
			return;
		}

		foreach ($domPlugins as $domPlugin) {
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
			$plugin->getPlugin($this); // initialize plugin
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

		if ($fileName === false) {
			return;
		}

		if (!($dom = Xml::load($fileName))) {
			FatalError("Ошибка в файле настроек <em>$fileName</em>");
		}

		$domTriggers = $dom->xpath('/triggers//trigger');

		if (!is_array($domTriggers) || count($domTriggers) == 0) {
			FatalError('Не описаны триггеры проекта');
		}

		foreach ($domTriggers as $domTrigger) {
			$this->setTrigger($domTrigger->attributes(), $domTrigger->parent()->nodeName());
		}
	}

	private function setTrigger($attributes, $type) {
		$trigger = new PXTriggerDescription();
		$trigger->setAttrs($attributes);
		$trigger->setType($type);

		if (!isset($this->triggers[$type])) {
			$this->triggers[$type] = array();
		}

		$this->triggers[$type][$trigger->getName()] = $trigger;
	}

	/**
	 * Пользовательская функция для регистрации триггеров не через конфиг triggers.xml, а в процессе работы системы
	 *
	 * @param  string $type - тип триггера
	 * @param  array $args - массив с параметрами триггера, имя => значение
	 * @return $this
	 */
	function registerTrigger($type, array $args) {
		foreach ($args as $name => &$value) {
			$value = Xml::attributePrototype($name, $value); //XML style attributes compatibility
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
	 * @param  string $engineClass каталог с файлами
	 * @return static|null serialized engine
	 */
	private static function loadFromCache($engineClass) {
		$cacheFile = self::_cacheFilename($engineClass);

		if (!file_exists($cacheFile)) {
			return null;
		}

		$tmp = stat($cacheFile);
		$cacheMtime = $tmp['mtime'];

		foreach (self::configDirs() as $configDir) {
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

	private static function _cacheFilename($name) {
		return CACHE_PATH . '/config/' . strtolower($name);
	}

	private static function parseIniFile($file) {
		return (array)parse_ini_file($file, true);
	}

	private static function parseIniFiles($files) {
		/* we must merge array in reverse order: libpp + local + site,
		 * so local can redefine libpp settings, and site can override local */

		$result = self::parseIniFile(array_pop($files));

		while (count($files)) {
			$tmp = self::parseIniFile(array_pop($files));

			foreach ($tmp as $k => $v) {
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
	 * Searches for the first file in order - site, local, libpp
	 * @param string $filename
	 * @param bool $required
	 * @return string|bool
	 */
	private static function findConfigFile($filename, $required = true) {
		$files = self::findFilesUnless($filename, function ($files) {
			return count($files) > 0;
		});

		if ($files) {
			return $files[0];
		}

		if ($required) {
			FatalError('Отсутствует файл настроек ' . $filename);
		}

		return false;
	}

	/**
	 * Searches for all files at site, local, libpp dirs.
	 * @param string $filename
	 * @param bool $required
	 *
	 * @return array|null
	 */
	private static function findConfigFiles($filename, $required = true) {
		$files = self::findFilesUnless($filename);
		if (!empty($files)) {
			return $files;
		}

		if ($required) {
			FatalError('Отсутствует файл настроек ' . $filename);
		}

		return null;
	}

	/*
	 * function is_enough($files_array) { <return TRUE when enough files collected> }
	 * @param string $filename
	 * @param callable $is_enough
	 */
	private static function findFilesUnless($filename, $is_enough = NULL) {
		$files = array();

		foreach (self::configDirs() as $configDir) {
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
		return [
			APPPATH . 'config/',
			BASEPATH . '/site/etc/',
			BASEPATH . '/local/etc/',
			BASEPATH . '/libpp/etc/'
		];
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

	/**
	 * @throws Exception
	 */
	function __wakeup() {
		EnvLoader::inject();

		foreach ($this->reinitialize as $name => $bool) {
			foreach ((array)$this->{$name} as $o) {
				if (!$o instanceof InitializableDescriptionInterface) {
					throw new \Exception(
						sprintf(
							"Class: %s is not instanceof IInitializableDescription",
							get_class($o)
						)
					);
				}

				$o->initialize($this);
			}
		}
	}
}
