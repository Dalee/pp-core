<?
class PXEngineAction extends PXEngineInterface {
	var $nextLocation;

	function html() {
		return '';
	}

	function initLayout() {
		$this->layout = new PXLayoutNull();
	}

	function runModules() {
		NLProfiler('RUN MODULES', 'BEGIN');
		$this->nextLocation = $this->modules->activateModules('useraction', $this->app, $this->db, $this->request, $this->user, $this->layout, $this->container);
		NLProfiler('RUN MODULES', 'END');
	}

	function profilerStart() {
	}

	function profilerStop() {
	}

	function redirect() {
		$nextLocation = $this->nextLocation;

		if (empty($nextLocation)) {
			$nextLocation = $this->request->GetHttpReferer();
		}

		if (empty($nextLocation)) {
			$nextLocation = '/';
		}

		Finalize($nextLocation);
	}
}
?>
