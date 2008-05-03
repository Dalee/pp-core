<?
class PXEngineAPI {
	var $area;
	var $settings;

	function __construct($area, $settings) {
		$this->area     = $area;
		$this->settings = $settings;

		$this->app      = PXEngineObjects::getApp();
		$this->request  = PXEngineObjects::getRequest();
		$this->user     = PXEngineObjects::getUser();
		$this->db       = PXEngineObjects::getDb();
		$this->layout   = PXEngineObjects::getLayout();
	}

	function index() {
	}

	function action() {
	}

	function json() {
	}

	function indexAdmin() {
		$this->layout->assignError('INNER.1.0', 'Функция <em>indexAdmin</em> данного модуля не определена');
	}

	function actionAdmin() {
		FatalError("Функция <EM>actionAdmin</EM> данного модуля не определена");
	}

	function popupAdmin() {
		$this->layout->assignError('OUTER.CONTENT', 'Функция <em>popupAdmin</em> данного модуля не определена');
	}
}

class PXEngineOldAPI extends PXEngineAPI {
	function index() {
		$this->userIndex($this->app, $this->request, $this->user, $this->db, $this->layout);
	}

	function action() {
		$this->userAction($this->app, $this->request, $this->user, $this->db);
	}

	function json() {
		$this->userJson($this->app, $this->request, $this->user, $this->db);
	}

	function indexAdmin() {
		$this->adminIndex($this->app, $this->request, $this->user, $this->db, $this->layout);
	}

	function actionAdmin() {
		$this->adminAction($this->app, $this->request, $this->user, $this->db);
	}

	function popupAdmin() {
		$this->adminPopup($this->app, $this->request, $this->user, $this->db, $this->layout);
	}
}

class PXAbstractModule extends PXEngineOldAPI {
	function PXAbstractModule($area, $settings) {
		parent::__construct($area, $settings);
	}

	function buildAdminUrl() {
		return str_replace('action.phtml', '', $_SERVER['SCRIPT_URL']);
	}

	function buildAdminIndexUrl() {
		return str_replace('action.phtml', '', $_SERVER['SCRIPT_URL']).'?area='.$this->area;
	}

	function adminIndex(&$app, &$request, &$user, &$db, &$layout) {
	}

	function adminPopup(&$app, &$request, &$user, &$db, &$layout) {
	}

	function adminAction(&$app, &$request, &$user, &$db) {
	}

	function userIndex(&$app, &$request, &$user, &$db, &$layout, &$tree, &$objects, &$subObjects, &$references, &$heap, $currentSid, $currentCid, $currentCtype, $currentSCid, $currentSCtype, $rootId, $pathId) {
	}

	function userAction(&$app, &$request, &$user, &$db) {
	}

	function userJson(&$app, &$request, &$user, &$db) {
	}
}

class RTAbstractModule extends PXAbstractModule {
	function RTAbstractModule($area, $settings) {
		$this->PXAbstractModule($area, $settings);
	}
}

?>
