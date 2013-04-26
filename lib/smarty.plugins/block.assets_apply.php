<?php

function smarty_block_assets_apply ($params, $content, &$smarty) {

	if (empty($content)) {
		return;
	}

	$assets = PXHTMLAssets::getInstance();
	if (!$assets->delayed_print) {
		return $content;
	}

	return $assets->applyDelayed($content);

}
