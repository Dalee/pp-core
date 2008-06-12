<?php
/**
 * Флешка
 *
 * storageType flash
 * displayType FLASH|width|height
 *             width и height не используются
 */
class PXDisplayTypeFlash extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = (isset($object[$k]) && is_array($object[$k]) && isset($object[$k]['path']) && !is_null($object[$k]['path']))
			? $object[$k]
			: array('width'=>null, 'height'=>null, 'path'=>null);

		return NLAbstractHTMLForm::BuildFlash($k, $value);
	}


	static function buildCell($field, $object, $param=NULL) {
		return PXDisplayTypeFlash::buildCellHtml($object[$field->name], $field->name.$object['id']);
	}

	static function buildCellHtml($value, $id) {
		if(!isset($value['path']) ) {
			return '';
		}

		$html  = '<div class="imagepreview image-in-cell"><div class="small">';
		$html .= '<script type="text/javascript">ShowFlash(\'i/l.swf?path='.$value['path'].'\', 50, 50, \''.$id.'src\');</script>';
		$html .= '</div>'.$value['width'].'x'.$value['height'].'</div>';

		return $html;
	}

	static function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetFileVar($field->name);
	}
}
?>
