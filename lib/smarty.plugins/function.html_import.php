<?php

/**
 * Display tag for inclusion local style or javescript files with asset id modifier to manipulate client caching
 *
 * Params:
 * - tag (string) - type of tag
 * - src (string) - path to local file
 * - extra params will be appended as tag attributes
 */
function smarty_function_html_import($params, &$smarty) {
	$tags = array(
		'css'    => '<link rel="stylesheet" type="text/css" href="%s" %s />',
		'script' => '<script type="text/javascript" src="%s" %s></script>'
	);

	if(empty($params['src']) || empty($tags[$params['tag']])) {
		return;
	}

	$extra_params = array();
	$extra_attributes = "";
	$minified = false;

	foreach($params as $param => $value){
		switch($param){
			case 'tag':
			case 'src':
				break;
			case 'min':
			case 'minified':
				$minified = $value;
				break;
			default:
				$extra_params[] = sprintf(' %s="%s" ', $param, $value);
		}
	}

	if (sizeof($extra_params) != 0){
		$extra_attributes = implode(' ', $extra_params);
	}

	$asset_id = '';

	if (strpos($params['src'], 'http') !== 0) {
		foreach (array('/site/htdocs/', '/local/htdocs/', '/libpp/htdocs/') as $localpath) {
			$localfile = BASEPATH . $localpath . $params['src'];
			if ($minified) {
				$minlocalfile = pathinfo($localfile, PATHINFO_FILENAME) . '.min.' . pathinfo($localfile, PATHINFO_EXTENSION);
				if (file_exists($minlocalfile)) {
					$asset_id = sprintf('?%s=%1$s', filemtime($minlocalfile));
					break;
				}
			}
			if (file_exists($localfile)) {
				$asset_id = sprintf('?%s=%1$s', filemtime($localfile));
				break;
			}
		}
	}

	printf($tags[$params['tag']], $params['src'] . $asset_id, $extra_attributes);
}

?>
