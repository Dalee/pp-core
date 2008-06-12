<?php
class PXRequestUserJson extends PXRequestUser {
	var $ajax;

	function PXUserJsonRequest() {
		include_once 'Subsys_JsHttpRequest/lib/Subsys/JsHttpRequest/Php.php';
		$this->ajax =  new Subsys_JsHttpRequest_Php(DEFAULT_CHARSET);
		$this->ajax->RESULT = array();

		if (!isset($_GET['charcheck'])) {
			$_GET['charcheck'] = myconv('k', DEFAULT_CHARSET, 'ÊÃÕËÅÎ');
		}

		$this->PXRequest();

		if(isset($_REQUEST['get-js-code'])) {
			header("X-Accel-Expires: 3600");

			ob_end_clean();
			echo myconv('w', 'k', $this->ajax->getJsCode());
			exit;
		}
	}

	function appendResult($varName, $value) {
		$this->ajax->RESULT[$varName] = $value;
	}

	function getArea() {
		return isset($_GET['area']) ? $_GET['area'] : NULL;
	}

	function setResult($result) {
		$this->ajax->RESULT = $result;
	}
}
?>