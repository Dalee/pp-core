<?php
/**
 * Выпадающий список с деревом объектов этого PXTypeDescription типа (как правило поле pid)
 *
 * storageType integet
 * displayType SELFPARENTDROPDOWN|width
 *             width не используется
 */
class PXDisplayTypeSelfparentdropdown extends PXDisplayTypeParentdropdown {
	static function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		return NLAbstractHTMLForm::BuildDropDown($field->name, $value, $param['selfParents']);
	}
}
?>