<?php

class PXWidgetFieldGroupsTableView extends PXWidgetFieldGroups
{
    public function __construct($groupName, $group, &$dType, &$object)
    {
        parent::__construct($groupName, $group, $dType, $object);
    }

    public function html()
    {
        $html = '<table class="mainform">';
        if (mb_strlen((string) $this->groupName)) {
            $html .= '<caption>' . $this->groupName . '</caption>';
        }
        foreach ($this->group as $field) {
            $html .= $field->displayType->buildRow($field, $this->object, $this->params);
        }
        $html .= '</table>';
        return $html;
    }
}

