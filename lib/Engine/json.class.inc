<?
class PXEngineJSON extends PXEngineInterface {
	var $result;

	function initLayout() {
		$this->layout = new PXLayoutNull();
	}

	function initRequest() {
		$this->request = new PXUserJsonRequest();
	}

	function runModules() {
		NLProfiler('RUN MODULES', 'BEGIN');
		$this->result = $this->modules->activateModules('userjson', $this->app, $this->db, $this->request, $this->user, $this->layout, $this->container);
		NLProfiler('RUN MODULES', 'END');
	}

	function saveResult() {
		$result = $this->result;

		if(!is_array($result)) {
			$result = array();
		}

		$this->request->setResult($result);
	}
}
?>
