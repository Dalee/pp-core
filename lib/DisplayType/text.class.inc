<?php
/**
 * Текстовое поле
 *
 * storageType string, integer или float
 * displayType TEXT|width|height|cell_maxlength
 *             width не используется
 *             если height = 0, то строится <input type="text">
 *             если height > 0, то строится <textarea> и height это ее высота в пикселях
 *             cell_maxlength определяет максимальное число символов, выводимых в ячейке
 */
class PXDisplayTypeText extends PXDisplayType {
	function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		$width = isset($field->displayTypeArgs[1]) ? (int)$field->displayTypeArgs[1] : 0;
		$maxlength = isset($field->displayTypeArgs[2]) ? (int) $field->displayTypeArgs[2] : NULL;

		return NLAbstractHTMLForm::BuildText($field->name, $value, $width, $maxlength);
	}

	function buildCell($field, $object, $param=NULL) {
		$trunc = isset($field->displayTypeArgs[2]) && (int)$field->displayTypeArgs[2] > 0 ? (int)$field->displayTypeArgs[2] : 50;
		$value = strip_tags($object[$field->name]);
		$value = strlen($value) > $trunc ? substr($value, 0, $trunc).'&hellip;' : $value;
		
		return $this->getSubDirectoryIfExists($field, $object, $value);
	}
}
?>
