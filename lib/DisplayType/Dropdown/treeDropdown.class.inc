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
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name], ENT_COMPAT|ENT_HTML401, DEFAULT_CHARSET) : NULL;
		$referencedTree =  $field->values->GetTree();
	
		return NLAbstractHTMLForm::BuildDropDown($field->name, $value, $this->getListFromField($field));
	}

	protected function getListFromField($field) {
		$tree = $field->values->GetTree();
		return $tree ? $tree->GetPlainTree(0) : array();
	}
}
