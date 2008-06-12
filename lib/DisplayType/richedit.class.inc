<?php
/**
 * Визуальный редактор
 *
 * storageType string
 * displayType RICHEDIT|width|height
 *             width не используется
 *             height это высота в пикселях
 */
class PXDisplayTypeRichedit extends PXDisplayTypeText {
	static function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		return NLAbstractHTMLForm::BuildRichEdit($field->name, $value, $field->displayTypeArgs[1]);
	}
}
?>
