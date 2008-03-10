<?php
require_once 'index.class.inc';
require_once 'action.class.inc';
require_once 'json.class.inc';
require_once 'sbin.class.inc';

require_once 'Admin/index.class.inc';
require_once 'Admin/popup.class.inc';
require_once 'Admin/action.class.inc';

class PXEngineInterface {
	var $app;             // Объект с настройками и справочниками
	var $db;
	var $user;            // Пользователь
	var $request;
	var $layout;          // Шаблонизатор
	var $container;
	var $modules;

	function __construct() {
		$this->profilerStart();
	}

	function init() {     // Инициализация основных объектов приложения
		NLProfiler('INIT', 'BEGIN');
		$this->initApplication();
		$this->initDatabase();
		$this->initRequest();
		$this->initUser();

		$this->initLayout();
		$this->layout->setApp($this->app);

		$this->user->setDb($this->db);
		$this->user->setApp($this->app);
		$this->user->setRequest($this->request);
		$this->user->checkAuth();

		$this->db->LoadDirectoriesAutomatic($this->app->directory);

		$this->initContainer();
		$this->initModules();

		NLProfiler('INIT', 'END');
	}

	function initApplication() {
		$this->app = PXApplication::getInstance(BASEPATH);
	}

	function initDatabase() {
		$this->db = new PXDataBase($this->app, false);
	}

	function initRequest() {
		$this->request = new PXRequest();
	}

	function initUser() {
		$this->user = new PXUser();
	}

	function initLayout() {
		$this->layout = new PXUserHTMLLayout();
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

	function fillLayout() {
		$layout = $this->layout;

		// Передача данных в шаблонизатор
		$layout->assign('app',            $this->app,     true);
		$layout->assign('user',           $this->user,    true);
		$layout->assign('request',        $this->request, true);

		$layout->assignRequest($this->request);

		$layout->assignArray($this->container, true);

		$layout->assign('requestHost',    $this->request->getHttpHost());
		$layout->assign('requestUri',     $_SERVER['REQUEST_URI']);
		$layout->assign('requestReferer', $this->request->getHttpReferer());
		$layout->assign('requestPath',    $_SERVER['SCRIPT_NAME']);
	}

	function sendHeaders() {
		if(!headers_sent()) {
			return;
		}

		if ($this->container['error'] == 404) {
			if (function_exists('log404')) {
				log404($request);
			}

			/*
			header("HTTP/1.1 404");

			we can't send empty header.
			for more details see http://forums.iis.net/t/1146547.aspx
			*/
		}

		if (function_exists('sendheaders')) {
			sendheaders();
		}
	}

	function html() {
		$this->layout->display();
	}

	function runModules() {
		NLProfiler('RUN MODULES', 'BEGIN');
		$this->modules->activateModules('userindex', $this->app, $this->db, $this->request, $this->user, $this->layout, $this->container);
		NLProfiler('RUN MODULES', 'END');
	}

	function profilerStart() {
		NLProfiler('TOTAL', 'BEGIN');
	}

	function profilerStop() {
		NLProfiler('TOTAL', 'END');
		$this->layout->assign('NLProfiler', NLProfiler(NULL, 'ALL'));
	}
}
?>
