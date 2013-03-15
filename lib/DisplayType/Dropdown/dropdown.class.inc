<?php
/**
 * Выпадающий список
 *
 * storageType как правило integer, иногда string
 * displayType DROPDOWN|width
 *             width не используется
 */
class PXDisplayTypeDropdown extends PXDisplayType {
	protected static $trees = array();
	
	var $format;
	var $parentFormat;
	
	function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name], ENT_COMPAT|ENT_HTML401, DEFAULT_CHARSET) : NULL;
		return NLAbstractHTMLForm::BuildDropDown($field->name, $value, $field->values->GetList());
	}

	function buildCell($field, $object, $param=NULL) {
		if (!isset($field->values->displayField)) {
			return '';
		}
		$displayField = $field->values->displayField;
		$value = $object[$field->name];
		return isset($field->values->values[$value][$displayField]) ? $field->values->values[$value][$displayField] : '';
	}

	function buildTreeHover($field, $object, $param=NULL) {
		return $this->buildCell($field, $object, $param);
	}

	
	public static function getFormats(&$param) {
		$format = '';
		$parentFormat = '';
		if (count($param) && isset($param['datatype']) && is_object($param['datatype'])) {
			$format        = $param['datatype']->id;
			$parentFormat  = $param['datatype']->parent;
		}
		return array($format,$parentFormat);
	}
	
	public static function getParentTree(&$object,$parentFormat) {
		if(!isset(self::$trees[$parentFormat])){
			$db  = PXRegistry::getDb();
			$app = PXRegistry::getApp();
			
			self::$trees[$parentFormat] = $db->getObjects($app->types[$parentFormat], NULL, DB_SELECT_FULL_TREE | DB_SELECT_WITHOUT_EXTERNALS);
		}
		
		return self::$trees[$parentFormat];
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
