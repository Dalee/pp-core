<?php
/**
 * Флешка
 *
 * storageType flash
 * displayType FLASH|width|height
 */
class PXDisplayTypeFlash extends PXDisplayTypeImage {
	function getAdminWidget($k, $value) {
		return NLAbstractHTMLForm::BuildFlash($k, $value);
	}

	function buildCell($field, $object, $param=NULL) {
		$maxWidth = $this->getMaxWidth($field);
		$maxHeight = $this->getMaxHeight($field);
		return $this->buildCellHtml($object[$field->name], $field->name.$object['id'], $maxWidth, $maxHeight);
	}

	function buildCellHtml() {
		list($value, $id, $maxWidth, $maxHeight) = func_get_args(); // PHP54 args contract violation
		if(!isset($value['path'])) {
			return '';
		}
		
		$html  = '<div class="imagepreview image-in-cell"><div class="small" style="width: '.$maxWidth.'px; height: '.$maxHeight.'px;">';
		$html .= '<script type="text/javascript">ShowFlash(\'i/l.swf?path='.$value['path'].'\', '.$maxWidth.', '.$maxHeight.', \''.$id.'src\');</script>';
		$html .= '</div>'.$value['width'].'x'.$value['height'].'</div>';

		return $html;
	}

	function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetFileVar($field->name);
	}
}
?>