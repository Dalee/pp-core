<?php
function smarty_function_createpath($params, &$smarty) {
	if($params['tree'] && $params['id']) {
		$path = createPathByParentId($params['tree'], $params['id']);
		if (empty($params['assign'])) {
			return $path;
		}
		
		$smarty->assign($params['assign'], $path);
	}
}
?>
