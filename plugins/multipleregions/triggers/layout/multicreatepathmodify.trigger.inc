<?php

class PXTriggerLayoutMultiCreatepathmodify extends PXAbstractLayoutTrigger
{

    public function OnBeforePathCreated($tree, $id)
    {

        // check for not existance node in tree
        if (!(isset($tree->leafs) && empty($tree->leafs[$id]))) {
            return;
        }

        if (isset($tree->__multiCreatePathModifyCache__[$id])) {
            $id = $tree->__multiCreatePathModifyCache__[$id];
            return;
        }

        if (!isset($tree->__multiCreatePathModifyCache__)) {
            $tree->__multiCreatePathModifyCache__ = [];
        }

        $reflexField = PXMultipleRegionsReflexer::REFLEX_FIELD;

        foreach ($tree->leafs as $k => $l) {

            // skip empty artifacts
            if ($k === '' && is_null($l)) {
                continue;
            }

            // looking for cloned node
            if ($l->id == 0 || empty($l->content[$reflexField])) {
                continue;
            }

            $reflexId = $l->content[$reflexField];
            if ($reflexId == $id) {
                $tree->__multiCreatePathModifyCache__[$id] = $l->id;
                $id = $l->id; //set new value
                return;
            }
        }

        $tree->__multiCreatePathModifyCache__[$id] = $id;
    }
}
