<?php

class NLGetData extends NLHTTPVarsData
{
    public function __construct($needToStripTags = false)
    {
        parent::__construct($needToStripTags);
        $this->raw = (array)$_GET;
        $this->_NormalizeData();
    }
}
?>
