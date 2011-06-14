<?php

/**
 * Display JS-tag that loads jQuery
 * from Google AJAX CDN with desired version.
 *
 * Params:
 * - v (string) - version. May 1.x, 1.x.x, - load latest.
 * - dev (boolean) - load uncompressed version
 */
function smarty_function_jquery($params, &$smarty) {
	if (empty($params['v'])) {
	//	load latest 1.x.x
		$params['v'] = '1';
	}
	$src = sprintf("https://ajax.googleapis.com/ajax/libs/jquery/%s/jquery.%sjs", $params['v'], empty($params['dev']) ? 'min.' : '');
	echo '<script type="text/javascript" src="' . $src . '"></script>';
}

?>
