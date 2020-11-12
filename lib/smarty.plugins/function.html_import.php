<?php

/**
 * Display tag for inclusion local style or javescript files with asset id modifier to manipulate client caching
 * @todo Split ::import call to separate assets-api calls depends on parameters
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
	return PXHTMLAssets::getInstance()->import($params);
}
