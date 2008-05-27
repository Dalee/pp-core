<?
class PXEngineAdminPopup extends PXEngineAdminIndex {
	function initLayout() {
		$this->layout = new PXAdminHTMLLayout('popup', $this->app->types);
	}

	function initModules() {
		$this->modules = $this->app->getAvailableModules($this->user->level);
		$this->area = $this->request->getArea(current(array_keys($this->modules)));
	}

	function fillLayout() {
		$this->layout->setGetVarToSave('area', $this->area);
	}

	function runModules() {
		NLProfiler('RUN MODULES', 'BEGIN');

		if (!$this->user->isAdmin()) {
			return $this->layout->assignError('OUTER.CONTENT', 'Нет доступа');
		}

		if(!$this->checkArea($this->area)) {
			return;
		}

		$this->fillLayout();

		$module = $this->modules[$this->area];
		$instance = $module->getModule();

		$this->layout->append('OUTER.CONTENT', $instance->adminPopup());

		NLProfiler('RUN MODULES', 'END');
	}
}
?>
