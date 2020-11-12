<?php

use \PP\Lib\Html\Layout\LayoutAbstract;

class PXAdminList extends PXAdminWidget
{
    public $_list;
    public $_varName;
    public $_selected;
    public $_getData;

    public function __construct($list)
    {
        $this->_list = $list;
        $this->_varName = '';
        $this->_selected = '';
        $this->_getData = [];
    }

    public function setVarName($varName)
    {
        $this->_varName = $varName;
    }

    public function setSelected($selected)
    {
        $this->_selected = $selected;
    }

    public function setGetData($getData)
    {
        if (is_array($getData)) {
            $this->_getData = $getData;
        }
    }

    public function html()
    {
        $html = '<ul class="list">';
        foreach ($this->_list as $k => $v) {
            $html .= '<li';
            if ($k == $this->_selected) $html .= ' class="selected"';
            $html .= '><a href="' . $this->_BuildHref($this->_varName, $k) . '">' . $v . '</a>';
            $html .= '</li>';
        }
        $html .= '</ul>';
        return $html;
    }

    public function _buildHref($key, $val = NULL)
    {
        return LayoutAbstract::buildHref($key, $val, $this->_getData);
    }

}

