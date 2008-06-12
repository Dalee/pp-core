<?php
/**
 * Выпадающий список с деревом разделов (как правило поле parent)
 *
 * storageType integet
 * displayType PARENTDROPDOWN|width
 *             width не используется
 */
class PXDisplayTypeParentdropdown extends PXDisplayTypeDropdown {
	static function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		return NLAbstractHTMLForm::BuildDropDown($field->name, $value, $param['parents']);
	}
}
?>
