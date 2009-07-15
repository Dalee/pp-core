<?php
require_once 'html.dev.class.inc';
require_once 'text.dev.class.inc';

require_once 'html.class.inc';
require_once 'text.class.inc';

define('NL_FATAL_ERROR', 16384);

function FatalError($message='') {
	PXErrorReporter::fatalError($message);
}

set_error_handler(array('PXErrorReporter', 'catchError'));
register_shutdown_function(array('PXErrorReporter', 'shutdown'));

// abstract
class PXErrorReporter {
	// protected
	var $errors = array();

	// static
	function &getInstance() {
		static $instance;

		if(is_null($instance)) {
			$cls  = 'PXErrorReporter';
			$cls .= CLI ? 'Text' : 'HTML';
			$cls .= ini_get('display_errors') ? 'Dev' : '';

			$instance = new $cls();
		}

		return $instance;
	}

	static
	function catchError($code, $message, $file, $line) {
		$reporter =& PXErrorReporter::getInstance();

		if ($code & (NL_FATAL_ERROR | error_reporting())) {
			$reporter->_addError($code, $message, $file, $line);
			$error = $reporter->getLastError();
			$reporter->warning($error);
		}

		return $code;
	}

	// static
	function fatalError($message) {
		$reporter =& PXErrorReporter::getInstance();

		$trace = PXTrace();
		$reporter->_addError(NL_FATAL_ERROR, $message, $trace[1]['file'], $trace[1]['line'], 3);
		$error = $reporter->getLastError();
		$reporter->fatalWarning($error);
		exit;
	}

	// static
	function shutdown() {
		$reporter =& PXErrorReporter::getInstance();
		if($reporter->hasErrors()) {
			$reporter->sendReport();
		}
	}

	function sendReport() {
		// pass
	}

	function _getErrorType($code) {
		static $errorTypes = array(
			E_ERROR           => 'Error',
			E_WARNING         => 'Warning',
			E_PARSE           => 'Parsing Error',
			E_NOTICE          => 'Notice',

			E_CORE_ERROR      => 'Core Error',
			E_CORE_WARNING    => 'Core Warning',

			E_COMPILE_ERROR   => 'Compile Error',
			E_COMPILE_WARNING => 'Compile Warning',

			E_USER_ERROR      => 'User Error',
			E_USER_WARNING    => 'User Warning',
			E_USER_NOTICE     => 'User Notice',

			NL_FATAL_ERROR    => 'Fatal Error'
		);

		return isset($errorTypes[$code]) ? $errorTypes[$code] : 'Unknown error type '.$code;
	}

	function _addError($code, $message, $file, $line, $traceFromLine=2) {
		$trace = PXTrace();

		$error = array(
			'type'    => $this->_getErrorType($code),
			'code'    => $code,
			'message' => $message,
			'file'    => $file,
			'line'    => $line,
			'trace'   => array_slice($trace, $traceFromLine)
		);

		$this->errors[] = $error;
	}

	function getLastError() {
		return $this->errors[sizeof($this->errors) - 1];
	}

	function isFirstError() {
		return sizeof($this->errors) == 1;
	}

	function hasErrors() {
		$reporter =& PXErrorReporter::getInstance();
		return sizeof($reporter->errors) > 0;
	}
}
?>
