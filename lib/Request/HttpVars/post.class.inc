<?php

class NLPostData extends NLHTTPVarsData
{
    public function __construct($needToStripTags = false)
    {
        parent::__construct($needToStripTags);
        $this->raw = (array)$_POST;
        $this->_NormalizeData();
    }

    public function setImageArray(&$retArray)
    {
        $tmpArray = $retArray['name'];
        $retArray['name'] = [];

        foreach ($tmpArray as $k => $v) {
            $retArray['name'][$k] = stripslashes((string) $v);

            if ($retArray['error'][$k] != 0 || $retArray['size'][$k] == 0) {
                unset($retArray['name'][$k]);
            }
        }
    }

    public function _GetImageVar($varName, $isArray = FALSE)
    {
        $retArray = [];

        if (!empty($_FILES[$varName]['name'])) {
            $retArray = $_FILES[$varName];

            $this->_Normalize($retArray);

            if ($isArray) {
                $this->setImageArray($retArray);
            } else {
                $retArray['name'] = stripslashes((string) $retArray['name']);

                if ($retArray['error'] != 0 || $retArray['size'] == 0) {
                    $retArray = [];
                }
            }
        }

        if (isset($this->raw[$varName . '_remove']) && is_array($this->raw[$varName . '_remove'])) {
            $retArray = array_merge($retArray, ['remove' => array_keys($this->raw[$varName . '_remove'])]);
        }

        return $retArray;
    }

    public function _GetFileVar($varName, $isArray = FALSE)
    {
        return $this->_GetImageVar($varName, $isArray);
    }
}
