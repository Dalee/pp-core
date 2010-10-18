<?php
function smarty_function_createpath($params, &$smarty) {
	$createdPath = "";
	if($params['tree'] && $params['id']) {
		$createdPath = createPathByParentId($params['tree'], $params['id']);
	}
	
	$app = PXRegistry::getApp();
	foreach($app->triggers->layout as $t) {
		$createdPath = $t->getTrigger()->OnAfterPathCreated($createdPath);
	}
	return $createdPath;
}
?>
