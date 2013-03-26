<?php

/**
 * Display tag for inclusion local style or javescript files with asset id modifier to manipulate client caching
 * @ todo: Move it to separate assets class and leave here just api calls
 *
 * Params:
 * - tag (string) - type of tag
 * - src (string) - path to local file
 * - noasset - skip this file for assets
 * - asset_group - custom asset bundle name
 * - print_asset - draw link to generated asset
 * - extra params will be appended as tag attributes
 */
function smarty_function_html_import($params, &$smarty) {

	static $imported = array();

	$tags = array(
		'css'    => '<link rel="stylesheet" type="text/css" href="%s" %s />',
		'script' => '<script type="text/javascript" src="%s" %s></script>'
	);

	$tags_inline = array(
		'css'    => "<style type=\"text/css\" %2\$s>\n%1\$s\n</style>",
		'script' => "<script type=\"text/javascript\" %2\$s>\n//<![CDATA[\n%1\$s\n//]]>\n</script>"
	);

	$asset_types = array(
		'css'    => 'css',
		'script' => 'js'
	);

	$asset_delimiters = array(
		'css'    => "\n",
		'script' => ';'
	);

	$allowed_paths = array(
		BASEPATH . '/site/htdocs/', 
		BASEPATH . '/local/htdocs/', 
		BASEPATH . '/libpp/htdocs/'
	);

	if ((empty($params['src']) && empty($params['print_asset'])) || empty($tags[$params['tag']])) {
		return;
	}

	$extra_params     = array();
	$extra_attributes = "";
	$asset_mode       = PXRegistry::getApp()->getProperty('CONFIG.ASSETS_ENABLED') && !isset($params['noasset']);
	$print_tag        = !$asset_mode;

	foreach($params as $param => $value) {
		switch($param) {
			case 'tag':
			case 'src':
			case 'noasset':
			case 'asset_group':
			case 'print_asset':
			case 'inline':
			case 'unique':
				break;
			default:
				$extra_params[] = sprintf(' %s="%s" ', $param, htmlspecialchars($value));
		}
	}

	if (sizeof($extra_params) != 0) {
		$extra_attributes = implode(' ', $extra_params);
	}

	$asset_id     = '';
	$assetH       = null; 
	$assets_group = isset($params['asset_group']) ? $params['asset_group'] : null;
	$assets_dir   = ''; //FIXME

	$inline       = isset($params['inline']) ? $params['inline'] : null;

	if ($asset_mode) {
		$assetH = PXHtmlAssetsManager::getInstance(BASEPATH . '/site/htdocs' . $assets_dir, $allowed_paths);
	}

	$is_bundle = false;
	switch (true) {
		case $inline && strpos($params['src'], 'http') !== 0:
			foreach ($allowed_paths as $localpath) {
				if (file_exists($localfile = $localpath . $params['src'])) {
					$params['content'] = file_get_contents($localfile);
					$print_tag = true;
					break;
				}
			}
			break;

		case $assetH && !empty($params['print_asset']):
			list($fullPath, $localPath, $mtime) = $assetH->makeAssetsBundle($asset_types[$params['tag']], $asset_delimiters[$params['tag']], $assets_group);
			if (!empty($localPath)) {
				$asset_id      = $mtime;
				$params['src'] = $assets_dir . $localPath;
				$print_tag    = true; //allow write result asset tag in asset mode
				$is_bundle = true;
			}
			break;

		case !empty($params['src']) && strpos($params['src'], 'http') !== 0:
			$print_tag = false;
			foreach ($allowed_paths as $localpath) {
				if (file_exists($localfile = $localpath . $params['src'])) {
					if ($assetH) {
						$assetH->addFileToBundle($localfile, $asset_types[$params['tag']], $assets_group);
						break;
					}
					$asset_id = filemtime($localfile);
					$print_tag = true;
					break;
				}
			}
			break;

		case !(empty($params['print_asset']) || $asset_mode):
			$print_tag = false; //skip empty print of {html_import print_asset=1 ...} when asset_mode turned off
			break;

		default:
			$print_tag = true;
	}

	if (!empty($params['unique']) && isset($imported[$params['src']])) {
		return;
	}
	$imported[$params['src']] = true;

	$out = '';
	if ($print_tag) {
		$cndSource = PXHtmlImageTag::getInstance()->buildCdnUrl($params['src']);
		$inline || $out = sprintf($tags[$params['tag']], $cndSource . ($asset_id && !$is_bundle ? sprintf('?%1$s=', $asset_id) : ''), $extra_attributes);
		$inline && $out = sprintf($tags_inline[$params['tag']], $params['content'], $extra_attributes);
	}

	return $out;
}
