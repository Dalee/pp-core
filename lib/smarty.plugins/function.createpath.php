<?php
function smarty_function_createpath($params, &$smarty) {
	if($params['tree'] && $params['id']) {
		return createPathByParentId($params['tree'], $params['id']);
	}
}
?>
