<?php

/**
 * Display tag for inclusion local style or javescript files with asset id modifier to manipulate client caching
 *
 * Params:
 * - tag (string) - type of tag
 * - src (string) - path to local file
 */
function smarty_function_html_import($params, &$smarty) {
	#TODO: add ability to set custom attributes like <link rel="stylesheet" media= ...
	$tags = array(
		'css' => '<link rel="stylesheet" type="text/css" href="%s" />',
		'script' => '<script type="text/javascript" src="%s"></script>'
	);
	
	if(empty($params['src']) || empty($tags[$params['tag']])) {
		return;
	}
	
	$asset_id = '';
	
	if (strpos($params['src'], 'http') !== 0) {
		foreach(array('/site/htdocs/', '/local/htdocs/', '/libpp/etc/') as $localpath) {
			if(file_exists($localfile = BASEPATH . $localpath . $params['src'])) {
				$asset_id = sprintf('?%s=%1$s', filemtime($localfile));
				break;
			}
		}
	}

	printf($tags[$params['tag']], $params['src'] . $asset_id);
}

?>
