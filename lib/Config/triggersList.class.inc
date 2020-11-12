<?php

final class PXTriggersList extends ArrayObject
{
    public function __get($triggerType)
    {
        return empty($this[$triggerType]) || !is_array($this[$triggerType]) ? [] : $this[$triggerType];
    }
}

?>
