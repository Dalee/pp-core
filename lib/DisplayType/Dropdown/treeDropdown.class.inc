<?php
/**
 * Выпадающий список с деревом объектов из связанного справочника(directory) указанного в аттрибуте source 
 *
 * storageType integet
 * displayType TREEDROPDOWN|width
 *             width не используется
 * 
 */

class PXDisplayTypeTreedropdown extends PXDisplayTypeDropdown {
	function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name], ENT_COMPAT|ENT_HTML401, DEFAULT_CHARSET) : NULL;
		$referencedTree =  $field->values->GetTree();
	
		return NLAbstractHTMLForm::BuildDropDown($field->name, $value, $referencedTree->getPlainTree(0));
	}
}
?>