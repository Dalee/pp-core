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
		$value      = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		$format     = $param['datatype']->id;
		$exceptions = array('struct', 'sgroup'); //todo: it must be fixed
		
		foreach($param['parents'] as $k => $v){
			@$allowed =& $param['tree']->leafs[$k]->content['allowed'];

			if(in_array($format, $exceptions) || isset($allowed[$format])){
				$param['parents'][$k] = array('title' => $v);
			} else {
				$param['parents'][$k] = array('title' => $v, 'disabled' => true);
			}
		}

		return NLAbstractHTMLForm::BuildParentDropDown($field->name, $value, $param['parents']);
	}
}

?>
