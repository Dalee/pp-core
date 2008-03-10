<?
function Close() {
	if (class_exists('NLErrorReporter') && NLErrorReporter::ErrorCount()) {
		exit();
	}

	print '<HTML>
		<HEAD>
			<SCRIPT language="JavaScript" type="text/javascript">
			function doIt() {
				window.close();
			}
			</SCRIPT>
		</HEAD>
		<BODY onLoad="doIt()">
			<BUTTON onClick="doIt()">Close</BUTTON>
		</BODY>
		</HTML>';
	exit();
}

function CloseAndRefresh() {
	if (class_exists('NLErrorReporter') && NLErrorReporter::ErrorCount()) {
		exit();
	}

	print '<HTML>
		<HEAD>
			<SCRIPT language="JavaScript" type="text/javascript">
			function doIt() {
				window.close();
				opener.location.href = opener.location.href;
			}
			</SCRIPT>
		</HEAD>
		<BODY onLoad="doIt()">
			<BUTTON onClick="doIt()">Close</BUTTON>
		</BODY>
		</HTML>';
	exit();
}

function Finalize($url) {
	switch (ini_get('display_errors')) {
		case 'on':
		case '1':
			if (class_exists('NLErrorReporter') && NLErrorReporter::ErrorCount()) {
				exit();
			}
			break;
	}

//	header('HTTP/1.0 301 Moved Permanetly');
//	header('HTTP/1.0 302 Found');
//	header('HTTP/1.0 303 See Other');
	header('Location: '.$url);
	exit();
}

function ListenInput($length = 255) {
	if (!isset ($GLOBALS['StdinPointer'])) {
		$GLOBALS['StdinPointer'] = fopen ("php://stdin","r");
	}

	$line = fgets ($GLOBALS['StdinPointer'], $length);

	return trim ($line);
}

?>
