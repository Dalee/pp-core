<?php
require_once 'objects.class.inc';

require_once 'index.class.inc';
require_once 'action.class.inc';
require_once 'json.class.inc';
require_once 'sbin.class.inc';

require_once 'Admin/index.class.inc';
require_once 'Admin/popup.class.inc';
require_once 'Admin/action.class.inc';

class PXEngineInterface {
	var $container;
	var $modules;

	function __construct() {     // Инициализация основных объектов приложения
		NLProfiler('INIT', 'BEGIN');

		$user = PXEngineObjects::getUser();

		$user->setDb(PXEngineObjects::getDb());
		$user->setApp(PXEngineObjects::getApp());
		$user->setRequest(PXEngineObjects::getRequest());
		$user->checkAuth();

		PXEngineObjects::getDb()->LoadDirectoriesAutomatic(PXEngineObjects::getApp()->directory);

		$this->initContainer();
		$this->initModules();

		NLProfiler('INIT', 'END');
	}

	function initContainer() {   // Инициализация необходимых данных и складывание их
		$this->container = array (
			'tree'          => NULL,                // Структура

			'pathId'        => array(),             // Хлебные крошки, состоящие из ID
			'currentSid'    => -1,                  // ID текущего раздела
			'rootId'        => -1,                  // ID корневого объекта струтуры


			'objects'       => array(),             // Объекты текущего раздела
			'currentCid'    => -1,                  // ID текущего объекта
			'currentCtype'  => -1,                  // Тип текущего объекта

			'subObjects'    => array(),             // Субобъекты текущего объекта
			'currentSCid'   => -1,                  // ID текущего субобъекта
			'currentSCtype' => -1,                  // Тип текущего субобъекта

			'references'    => array(),             // Связи между объектами
			'heap'          => array(),             // "Куча" объектов, участвующих в связях

			'urlFile'       => '',
			'error'         => 0                    // Код ошибки
		);
	}

	function initModules() {
		$this->modules = new PXModuleLoader();
	}
}
?>
