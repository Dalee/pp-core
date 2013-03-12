<?php
/**
 * Скрытое поле
 *
 * storageType может быть любым, но как правило integer
 * displayType HIDDEN
 */
class PXDisplayTypeHidden extends PXDisplayType {
	function buildRow($field, $object, $param=NULL) {
		$value = (isset($object[$field->name]) && !is_array($object[$field->name])) ? htmlspecialchars($object[$field->name]) : NULL;
		return NLAbstractHTMLForm::BuildHidden($field->name, $value);
	}

	function getRow($even = false) {
		return $even;
	}

	function preModifyObject($field, &$object, $param=NULL) {
		return (bool)$param['skipHidden'];
	}

	function buildCell($field, $object, $param=NULL) {
		$value = is_string($object[$field->name]) ? strip_tags($object[$field->name]) : $object[$field->name];
		return $this->getSubDirectoryIfExists($field, $object, $value);
	}
}
?>
