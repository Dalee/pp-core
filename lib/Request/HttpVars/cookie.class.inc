<?php

class NLCookieData extends NLHTTPVarsData
{
    public function __construct($needToStripTags = false)
    {
        parent::__construct($needToStripTags, true);
        $this->raw = (array)$_COOKIE;
        $this->_NormalizeData();
    }

    public function _GetVar($varName)
    {
        if (!isset($this->raw[$varName])) {
            return NULL;
        }

		return $this->raw[$varName];
    }
}
?>
