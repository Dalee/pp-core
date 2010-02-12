<?

function PXInputHandlerTypo($field, $argument) {
	require_once('Text/typographer.php');
	return TypoAllRecursive($argument);
}

?>
