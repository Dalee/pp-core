<?php
/**
 *
 *
 */
function smarty_function_img($params, &$smarty) {
	return PXHtmlImageTag::getInstance()->buildTag($params);
}

?>