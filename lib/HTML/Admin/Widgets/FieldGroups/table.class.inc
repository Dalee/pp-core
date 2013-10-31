<?php
class PXWidgetFieldGroupsTableView extends PXWidgetFieldGroups {
	function PXWidgetFieldGroupsTableView($groupName, $group, &$dType, &$object) {
		parent::PXWidgetFieldGroups($groupName, $group, $dType, $object);
	}
	
	function html(){
		$html = '<table class="mainform">';
		if(mb_strlen($this->groupName)) {
			$html .= '<caption>' . $this->groupName . '</caption>';
		}
		foreach($this->group as $field){
			$html .= $field->displayType->buildRow($field, $this->object, $this->params);
		}
		$html .= '</table>';
		return $html;
	}
}
?>
