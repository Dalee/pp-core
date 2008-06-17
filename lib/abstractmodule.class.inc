<?
class PXAbstractModule {
	function __construct($area, $settings) {
		$this->area       = $area;
		$this->settings   = $settings;

		$this->app        = PXRegistry::getApp();
		$this->request    = PXRegistry::getRequest();
		$this->user       = PXRegistry::getUser();
		$this->db         = PXRegistry::getDb();
		$this->layout     = PXRegistry::getLayout();
		$this->types      = PXRegistry::getTypes();

		$this->struct     = PXRegistry::getVar('struct');
		$this->content    = PXRegistry::getVar('content');
		$this->subContent = PXRegistry::getVar('subContent');
	}

	function buildAdminUrl() {
		return str_replace('action.phtml', '', $_SERVER['SCRIPT_URL']);
	}

	function buildAdminIndexUrl() {
		return str_replace('action.phtml', '', $_SERVER['SCRIPT_URL']).'?area='.$this->area;
	}

	function adminIndex() {
		$this->layout->assignError('INNER.1.0', 'Функция <em>adminIndex</em> данного модуля не определена');
	}

	function adminPopup() {
		$this->layout->assignError('OUTER.CONTENT', 'Функция <em>adminPopup</em> данного модуля не определена');
	}

	function adminAction() {
		FatalError("Функция <em>adminAction</em> данного модуля не определена");
	}

	function userIndex() {
	}

	function userAction() {
	}

	function userJson() {
	}
}


/**
 * Marked to remove soon.
 *
 * @depricated
 */
class RTAbstractModule extends PXAbstractModule {
	function RTAbstractModule($area, $settings) {
		parent::_construct($area, $settings);
	}
}

?>
