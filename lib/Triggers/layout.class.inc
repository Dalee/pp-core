<?php

abstract class PXAbstractLayoutTrigger extends PXAbstractTrigger
{
    public function OnBeforePathCreated($tree, $id)
    {
    }

    public function OnAfterPathCreated($createdPath)
    {
        return $createdPath;
    }
}
