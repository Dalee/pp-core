<?php

abstract class PXAbstractSearchTrigger extends PXAbstractTrigger
{
    public function onBeforeSearchStems($config, &$where)
    {
    }

    public function onBeforeSearchObjects($formatid, &$where)
    {
    }

    public function onAfterSearchObjects($formatid, &$row)
    {
    }

    public function onIndexCronGetSelectCols($formatid, &$cols)
    {
    }

    public function onIndexCronStemWeight($formatid, &$stemW, &$row)
    {
    }

    public function onIndexCronCanIndexRow($formatid, &$row, &$tree)
    {
        return true;
    }
}

