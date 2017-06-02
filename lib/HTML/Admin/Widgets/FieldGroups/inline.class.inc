<?php
class PXWidgetFieldGroupsInlineView extends PXWidgetFieldGroups {
	public function __construct($groupName, $group, &$dType, &$object) {
		parent::__construct($groupName, $group, $dType, $object);
	}

	function html(){
		static $callCount = 0;
		$html = '<table class="mainform">';
		$html .= '<tr'. (PXDisplayType::getRow() ? '' : ' class="even"') .'>';
		$html .= '<th>' . $this->groupName . '</th>';
		$html .= '<td>';
		foreach($this->group as $field){
			$this->params['id'] = $field->name . $callCount++;
			$html .= '<div class="field-group-inline">';
			$html .= sprintf('<label for="%s">%s</label>',$this->params['id'], $field->description);
			$html .= $field->displayType->buildInput($field, $this->object, $this->params);
			$html .= '</div>';
		}
		$html .= '</td>';
		$html .= '</tr>';
		$html .= '</table>';
		return $html;
	}
}
