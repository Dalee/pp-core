<?php
class PXErrorReporterHTMLDev extends PXErrorReporter {
	function fatalWarning($error) {
		$css = $this->isFirstError() ? $this->css() : '';

		$traceHTML = $this->traceToHtml($error['trace']);

		echo <<<HTML
			<html class="error-debug">
				<head>
					<title>{$error['message']} &#8212; Fatal Error</title>
					{$css}
				</head>

				<body>
					<h1>Error</h1>
					<p>Sorry, but fatal error occured at <strong>{$error['file']}</strong>, <strong>{$error['line']}</strong>, and your request can not be contented</p>
					<p>Error description:</p>
					<pre class="desc">{$error['message']}</pre>

					<div class="error-debug-show_table">
						<table class="error-trace">
							<tr>
								<th width="25%">����:</th>
								<th width="10%">������:</th>
								<th>�������:</th>
								<th>���������:</th>
							</tr>

							{$traceHTML}
						</table>
					</div>
				</body>
			</html>
HTML;
	}

	function warning($error) {
		if($this->isFirstError()) {
			echo $this->css();
		}

		$traceHTML = $this->traceToHtml($error['trace']);

		echo <<<HTML
			<div class="error-debug">
				<strong> {$error['type']}</strong>: {$error['message']} in file <strong>{$error['file']}</strong> on line <strong>{$error['line']}</strong>

				<a href="" onclick="this.parentNode.className += ' error-debug-show_table'; this.style.display = 'none'; return false;">trace</a>
				<a href="" onclick="this.parentNode.style.display = 'none'; return false;">hide</a>

				<table class="error-trace">
					<tr>
						<th width="25%">����:</th>
						<th width="10%">������:</th>
						<th>�������:</th>
						<th>���������:</th>
					</tr>

					{$traceHTML}
				</table>
			</div>
HTML;
	}

	function traceToHtml($trace) {
		return implode(
			"\n", 

			array_map(
				create_function(
					'$error',

					<<<TRACE_TO_HTML
						\$function  = str_replace('->', '&rarr;', \$error['function']);

						\$arguments = implode(
							', ',

							array_map(
								create_function(
									'\$a', 
									" return '<span class=\"'.\\\$a['type'].'\" title=\"'.\\\$a['type'].'\">'.\\\$a['value'].'</span>' ;"
								),

								\$error['args']
							)
						);

						return "
							<tr>
								<td>{\$error['file']}</td>
								<td class=\"line\">{\$error['line']}</td>
								<td>{\$function}()</td>
								<td>{\$arguments}</td>
							</tr>
						";
TRACE_TO_HTML
				),

				$trace
			)
		);
	}

	function css() {
		return <<<HTML
			<style type="text/css">
				.error-debug {
					color: #000000;
					background-color: #FFFFFF;
					font: normal normal small Georgia;
					padding: 1em;
				}

				div.error-debug {
					border: 3px solid #990000;
					margin: 1em 0;
				}

				table.error-trace {
					width: 100%;
					border-collapse: collapse;
					display: none;
					margin: 1em 0 0;
				}

				.error-debug-show_table table.error-trace {
					display: table;
				}

				table.error-trace th, table.error-trace td {
					border: 1px solid #000000;
					background-color: #FFFFFF;
					text-align: left;
					vertical-align: top;
					padding: 0.25em 0.5em;
				}

				table.error-trace td.line {
					text-align: right;
				}

				table.error-trace th {
					font-size: x-small;
					border: none;
				}

				table.error-trace span {
					font-size: x-small;
					color: #000000
				}

				.error-debug pre.desc {
					margin-left: 100px;
				}

				.error-debug .string  { color: #000066; }
				.error-debug .integer { color: #660000; }
				.error-debug .array   { color: #006600; }
				.error-debug .object  { color: #666600; }
				.error-debug .null    { color: #666666; }
			</style>
HTML;
	}
}
?>