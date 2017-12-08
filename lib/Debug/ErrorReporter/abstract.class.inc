<?php

require_once PPLIBPATH . 'Debug/ErrorReporter/html.class.inc';
require_once PPLIBPATH . 'Debug/ErrorReporter/text.class.inc';

define('NL_FATAL_ERROR', 16384);

function FatalError($message='') {
	if (ini_get('display_errors')) {
		trigger_error($message, E_USER_ERROR);
	}

	PXErrorReporter::fatalError($message);
}

if (!ini_get('display_errors')) {
	set_error_handler(array('PXErrorReporter', 'catchError'));
	register_shutdown_function(array('PXErrorReporter', 'shutdown'));
}

// abstract
class PXErrorReporter {
	// protected
	var $errors = array();

	var $errorsMap = array();

	static
	function &getInstance() {
		static $instance;

		if(is_null($instance)) {
			$cls  = 'PXErrorReporter';
			$cls .= CLI ? 'Text' : 'HTML';

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

	static
	function fatalError($message) {
		$reporter =& PXErrorReporter::getInstance();

		$trace = PXTrace();
		$reporter->_addError(NL_FATAL_ERROR, $message, $trace[1]['file'], $trace[1]['line'], 3);
		$error = $reporter->getLastError();
		$reporter->fatalWarning($error);
		exit;
	}

	static
	function shutdown() {
		$reporter =& PXErrorReporter::getInstance();
		if(PXErrorReporter::hasErrors()) {
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
			E_PARSE           => 'Parse Error',
			E_NOTICE          => 'Notice',

			E_CORE_ERROR      => 'Core Error',
			E_CORE_WARNING    => 'Core Warning',

			E_COMPILE_ERROR   => 'Compile Error',
			E_COMPILE_WARNING => 'Compile Warning',

			E_STRICT          => 'Strict Notice',
			E_RECOVERABLE_ERROR => 'Catchable fatal error',
//			E_DEPRECATED      => 'Run-time notice', // since 5.3.0

			E_USER_ERROR      => 'User Error',
			E_USER_WARNING    => 'User Warning',
			E_USER_NOTICE     => 'User Notice',
//			E_USER_DEPRECATED => 'User Warning (Deprecated)', // since 5.3.0

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
		$this->errorsMap[$error['file']][$error['line']][$error['code']] = true;
	}

	function getLastError() {
		return $this->errors[sizeof($this->errors) - 1];
	}

	function isFirstError() {
		return sizeof($this->errors) == 1;
	}

	static
	function hasErrors() {
		$reporter =& PXErrorReporter::getInstance();
		return sizeof($reporter->errors) > 0;
	}
}
?>
