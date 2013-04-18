<?php
/**
 * Выпадающий список с деревом объектов из связанного справочника(directory) указанного в аттрибуте source 
 *
 * storageType integer|intarray
 * displayType TREEDROPDOWN|height
 * 
 */

class PXDisplayTypeTreedropdown extends PXDisplayTypeDropdown {
	function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? array_map("htmlspecialchars", (array)$object[$field->name]) : NULL;
		$value = array_key_exists(0, (array)$value) && $value[0] == "NULL" ? "" : $value;
		$referencedTree =  $field->values->GetTree();

		$width   = @$field->displayTypeArgs[0];
		$fldname = $width > 1 ? $field->name.'[]': $field->name;

		return NLAbstractHTMLForm::BuildDropDown($fldname, $value, $referencedTree->getPlainTree(0),
			($width > 1 ? true : false), ($width > 1 ? $width : NULL));
	}
}
?>