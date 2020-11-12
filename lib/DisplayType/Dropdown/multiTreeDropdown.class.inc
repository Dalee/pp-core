<?php

class PXDisplayTypeMultiTreeDropDown extends PXDisplayTypeMultiDropdown {
	protected function getListFromField($field) {
		$tree = $field->values->GetTree();
		return $tree ? $tree->GetPlainTree(0) : [];
	}
}
