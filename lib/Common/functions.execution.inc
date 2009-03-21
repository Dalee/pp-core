<?php
function Close() {
	if (PXErrorReporter::hasErrors()) {
		exit();
	}

	print <<<HTML
	<html>
		<head>
			<script type="text/javascript">
				function closePage() {
					window.close();
				}
			</script>
		</head>

		<body onLoad="closePage()">
			<button onclick="closePage()">Close</button>
		</body>
	</html>
HTML;

	exit();
}

function CloseAndRefresh() {
	if (PXErrorReporter::hasErrors()) {
		exit();
	}

	print <<<HTML
	<html>
		<head>
			<script type="text/javascript">
				function closePage() {
					window.close();
					opener.location.href = opener.location.href;
				}
			</script>
		</head>

		<body onLoad="closePage()">
			<button onclick="closePage()">Close</button>
		</body>
	</html>
HTML;

	exit();
}

function Finalize($url) {
	require_once 'libpp/lib/response.class.inc';

	$response =& PXResponse::getInstance();
	$response->redirect($url);
}

function ListenInput($length = 255) {
	if (!isset ($GLOBALS['StdinPointer'])) {
		$GLOBALS['StdinPointer'] = fopen ("php://stdin","r");
	}

	$line = fgets ($GLOBALS['StdinPointer'], $length);

	return trim ($line);
}

?>
