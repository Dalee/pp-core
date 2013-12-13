<?php
/**
 * Выпадающий список с деревом объектов из связанного справочника(directory) указанного в аттрибуте source
 *
 * storageType integer
 * displayType TREEDROPDOWN|width
 *
 */

class PXDisplayTypeTreedropdown extends PXDisplayTypeDropdown {
	function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? $object[$field->name] : NULL;
		$referencedTree =  $field->values->GetTree();
	
		return NLAbstractHTMLForm::BuildDropDown($field->name, $value, $this->getListFromField($field));
	}

	protected function getListFromField($field) {
		$tree = $field->values->GetTree();
		return $tree ? $tree->GetPlainTree(0) : array();
	}
}
