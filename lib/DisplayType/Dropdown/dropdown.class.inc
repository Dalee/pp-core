<?php
/**
 * Выпадающий список
 *
 * storageType как правило integer, иногда string
 * displayType DROPDOWN|width
 *             width не используется
 */
class PXDisplayTypeDropdown extends PXDisplayType {
	var $format;
	var $parentFormat;
	
	function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		return NLAbstractHTMLForm::BuildDropDown($field->name, $value, $field->values->GetList());
	}

	function buildCell($field, $object, $param=NULL) {
		$displayField = $field->values->displayField;
		$value = $object[$field->name];
		return isset($field->values->values[$value][$displayField]) ? $field->values->values[$value][$displayField] : '';
	}
	
	function getFormats(&$param) {
		$format = '';
		$parentFormat = '';
		if (count($param) && isset($param['datatype']) && is_object($param['datatype'])) {
			$format        = $param['datatype']->id;
			$parentFormat  = $param['datatype']->parent;
		}
		return array($format,$parentFormat);
	}
	
	function getParentTree(&$object,$parentFormat) {
		$db  = PXRegistry::getDb();
		$app = PXRegistry::getApp();
		return $db->getObjects($app->types[$parentFormat], NULL, DB_SELECT_FULL_TREE); 
	}
	
	function getSelfParentTree(&$object,&$param) {
		$db  = PXRegistry::getDb();
		
		if (isset($param['datatype']->fields['parent']) && !is_null($object['parent'])) {
			return $db->getObjectsByParent($param['datatype'], NULL, $object['parent'], DB_SELECT_TREE);
		} else {
			return $db->getObjects($param['datatype'], NULL, DB_SELECT_TREE);
		}
	}
}
?>
