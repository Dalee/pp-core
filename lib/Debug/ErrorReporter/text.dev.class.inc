<?php
class PXErrorReporterTextDev extends PXErrorReporter {
	function _addError($code, $message, $file, $line, $traceFromLine=2) {
		parent::_addError($code, $message, $file, $line, $traceFromLine);
	}


	function warning($error) {
		$type = strtoupper($error['type']);
		echo "\n\n* {$type}: {$error['message']} in {$error['file']} on line {$error['line']}\n\n";
	}

	function fatalWarning($error) {
		echo "\n\n\n* FATAL ERROR";
		echo "\nSorry, but fatal error occured at {$error['file']}, {$error['line']}, and your request can not be contented";
		echo "\nError description: {$error['message']}";

		echo $this->traceToText($error['trace']);

		echo "\n\n";
	}

	function traceToText($trace) {
		$traceLineText = <<<TRACE_LINE
			return "\n".
				"\n Файл:     \t".\$error['file'].
				"\n Строка:   \t".\$error['line'].
				"\n Функция:  \t".\$error['function']."()".
				"\n Параметры:\t".\$arguments
			;
TRACE_LINE;

		return implode(
			"\n",

			array_map(
				create_function(
					'$error',

					<<<TRACE_TO_TEXT
						\$arguments = implode(
							', ',

							array_map(
								create_function(
									'\$a',
									" return \\\$a['value'] ;"
								),

								\$error['args']
							)
						);

						$traceLineText
TRACE_TO_TEXT
				),

				$trace
			)
		);
	}
}
?>
