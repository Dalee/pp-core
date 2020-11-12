<?php

/**
 * Display JS-tag that loads jQuery
 * from Popular AJAX CDN with desired version.
 *
 * Params:
 * - v (string) Version
 * - provider (enum: [yandex[, google[, microsoft]]]) CDN provider
 * - dev (boolean) - Load uncompressed version?
 */
function smarty_function_jquery($params, &$smarty) {
	$providers = [
		'microsoft' => '//ajax.aspnetcdn.com/ajax/jquery/jquery-%s.%sjs',
		'google' => '//ajax.googleapis.com/ajax/libs/jquery/%s/jquery.%sjs',
		'yandex' => '//yandex.st/jquery/%s/jquery.%sjs'
    ];
	if (empty($params['v']) || $params['v'] == '1.6') {
	//	load latest 1.x.x
		$params['v'] = '1.6.1';
	}
	$min = (empty($params['dev']) ? 'min.' : '');
	if (!array_key_exists($params['provider'], $providers)) {
		$params['provider'] = 'yandex';
	}
	$src = sprintf($providers[$params['provider']], $params['v'], $min);
	echo '<script type="text/javascript" src="' . $src . '"></script>' . "\n";
}

?>
