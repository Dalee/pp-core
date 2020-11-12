<?php

class PXBindingDescriptionStructType extends PXBindingDescriptionStruct
{

    public function __construct($rule, $pOrder)
    {
        parent::__construct($rule, $pOrder);
        $this->var = 'type';
        $this->value = $this->module;
    }

}
?>
