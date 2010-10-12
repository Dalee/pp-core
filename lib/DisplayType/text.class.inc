<?php
/**
 * Текстовое поле
 *
 * storageType string, integer или float
 * displayType TEXT|width|height|cell_maxlength|wrap
 *             width не используется
 *             если height = 0, то строится <input type="text">
 *             если height > 0, то строится <textarea> и height это ее высота в пикселях
 *             cell_maxlength определяет максимальное число символов, выводимых в ячейке
 *             wrap определяет тип переноса в textarea - physical (по умолчанию), virtual, hard, soft, off
 */
class PXDisplayTypeText extends PXDisplayType {
	function buildInput($field, $object, $param=NULL) {
		$value     = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		$height    = isset($field->displayTypeArgs[1])  ? (int)$field->displayTypeArgs[1]   : 0;
		$maxlength = !empty($field->displayTypeArgs[2]) ? (int)$field->displayTypeArgs[2]   : NULL;
		$wrap      = !empty($field->displayTypeArgs[3]) ? trim($field->displayTypeArgs[3])  : "physical";

		return NLAbstractHTMLForm::BuildText($field->name, $value, $height, $maxlength, $wrap);
	}

	function buildCell($field, $object, $param=NULL) {
		$trunc = isset($field->displayTypeArgs[2]) && (int)$field->displayTypeArgs[2] > 0 ? (int)$field->displayTypeArgs[2] : 50;
		$value = strip_tags($object[$field->name]);
		$value = strlen($value) > $trunc ? substr($value, 0, $trunc).'&hellip;' : $value;
		
		return $this->getSubDirectoryIfExists($field, $object, $value);
	}
}
?>
