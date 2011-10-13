<?php

class  PXTriggerLayoutMultiCreatepathmodify extends PXAbstractLayoutTrigger {
	function onBeforePathCreated($tree, &$id) {
		if (!isset($tree->leafs) || !empty($tree->leafs[$id])) return;

		$table        = (array) $tree->toTable();
		$reflex_field = PXMultipleRegionsReflexer::REFLEX_FIELD;

		foreach ($table as $k => $v){
			if (!empty($v[$reflex_field]) && $id == $v[$reflex_field]) {
				$id = $v['id'];
				break;
			}
		}
	}
}

?>
