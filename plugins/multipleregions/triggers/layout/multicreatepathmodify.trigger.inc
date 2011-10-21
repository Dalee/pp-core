<?php

class  PXTriggerLayoutMultiCreatepathmodify extends PXAbstractLayoutTrigger {

	function onBeforePathCreated($tree, &$id) {

		// check for not existance node in tree
		if (!(isset($tree->leafs) && empty($tree->leafs[$id]))) {
			return;
		}

		static $idCache = array();

		if (isset($idCache[$id])) {
			$id = $idCache[$id];
			return;
		}

		$reflexField = PXMultipleRegionsReflexer::REFLEX_FIELD;

		foreach ($tree->leafs as $l) {

			// looking for cloned node
			if ($l->id == 0 || empty($l->content[$reflexField])) {
				continue;
			}

			$reflexId = $l->content[$reflexField];
			if ($reflexId == $id) {
				$idCache[$id] = $l->id;
				$id = $l->id;
				break;
			}
		}
	}
}

?>
