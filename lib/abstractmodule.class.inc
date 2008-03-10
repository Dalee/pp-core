<?
class PXAbstractModule {
	var $area;
	var $settings;

	function PXAbstractModule($area, $settings) {
		$this->area     = $area;
		$this->settings = $settings;
	}

	function adminIndex(&$app, &$request, &$user, &$db, &$layout) {
		$layout->assignError('INNER.1.0', 'Функция <EM>adminIndex</EM> данного модуля не определена');
	}

	function adminPopup(&$app, &$request, &$user, &$db, &$layout) {
		$layout->assignError('OUTER.CONTENT', 'Функция <EM>adminPopup</EM> данного модуля не определена');
	}

	function adminAction(&$app, &$request, &$user, &$db) {
		FatalError("Функция <EM>adminAction</EM> данного модуля не определена");
	}

	function userIndex(&$app, &$request, &$user, &$db, &$layout, &$tree, &$objects, &$subObjects, &$references, &$heap, $currentSid, $currentCid, $currentCtype, $currentSCid, $currentSCtype, $rootId, $pathId) {
	}

	function userAction(&$app, &$request, &$user, &$db) {
	}

	function buildAdminUrl() {
		return str_replace('action.phtml', '', $_SERVER['SCRIPT_URL']);
	}

	function buildAdminIndexUrl() {
		return str_replace('action.phtml', '', $_SERVER['SCRIPT_URL']).'?area='.$this->area;
	}

}

class RTAbstractModule extends PXAbstractModule {
	function RTAbstractModule($area, $settings) {
		$this->PXAbstractModule($area, $settings);
	}

}

?>
