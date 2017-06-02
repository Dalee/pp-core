<?php
class PXWidgetFieldGroups extends PXAdminWidget {
	public function __construct($groupName, $group, &$dType, &$object) {
		$this->groupName = $groupName;
		$this->group     = $group;
		$this->object    = &$object;

		$this->params    =  array(
			'even'        => false,
			'datatype'    => &$dType
		);
	}

	function html(){
		FatalError("It's abstract class, use other field group view class");
	}
}
