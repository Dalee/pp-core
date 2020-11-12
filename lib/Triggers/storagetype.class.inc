<?php

abstract class PXAbstractStoragetypeTrigger extends PXAbstractTrigger
{
    public function __call($m, $a)
    {
        return true;
    }
}
