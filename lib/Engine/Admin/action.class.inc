<?php
class PXEngineAdminAction extends PXEngineInterface {
	var $nextLocation;

	function initLayout() {
		$this->layout = new PXLayoutNull();
	}

	function initModules() {
		$this->modules = $this->app->getAvailableModules($this->user->level);
		$this->area    = $this->request->getArea(current(array_keys($this->modules)));
	}

	function runModules() {
		NLProfiler('RUN MODULES', 'BEGIN');

		if (!isset($this->modules[$this->area])) {
			FatalError('Некорректный параметр area = <em>'.strip_tags($this->area).'</em>');
		}

		$module = $this->modules[$this->area];

		$instance = $module->getModule();
		$this->nextLocation = $instance->adminAction($this->app, $this->request, $this->user, $this->db);

		NLProfiler('RUN MODULES', 'END');
	}

	function redirect() {
		switch ($this->request->getAfterActionDeal()) {
			case 'close':
				closeAndRefresh();
				break;

			case 'back':
				$nextLocation = $this->nextLocation;
				$nextLocation = !is_null($nextLocation) ? $nextLocation : $this->request->getReferer();

				Finalize($nextLocation);
				break;

			default:
				break;
		}
	}
}
?>
