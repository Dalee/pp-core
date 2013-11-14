<?php
/**
 * Выпадающий список с деревом объектов этого PXTypeDescription типа (как правило поле pid)
 *
 * storageType integet
 * displayType SELFPARENTDROPDOWN|width
 *             width не используется
 */
class PXDisplayTypeSelfparentdropdown extends PXDisplayTypeParentdropdown {
	function buildInput($field, $object, $param=NULL) {
		$value    = isset($object[$field->name]) ? htmlspecialchars($object[$field->name], ENT_COMPAT|ENT_HTML401, DEFAULT_CHARSET) : NULL;
		$selfTree = parent::getSelfParentTree($object,$param);
		$selfTree = $selfTree->getPlainTree(getFromArray($object, 'id'));
		if(isset($param['root_title'])){
			$selfTree[NULL] = $param['root_title'];
		}
		
		return NLAbstractHTMLForm::BuildDropDown($field->name, $value, $selfTree);
	}
}
?>