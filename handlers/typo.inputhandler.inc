<?php

function PXInputHandlerTypo($field, $argument) {
	require_once PPLIBPATH . 'Text/typographer.php';
	return TypoAllRecursive($argument);
}
