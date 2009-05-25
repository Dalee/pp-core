<?php
/**
 * Картинка
 *
 * storageType image
 * displayType IMAGE|width|height
 *             width и height не используются
 */
class PXDisplayTypeImage extends PXDisplayType {
	function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = (isset($object[$k]) && is_array($object[$k]) && isset($object[$k]['path']) && !is_null($object[$k]['path']))
			? $object[$k]
			: array('width'=>null, 'height'=>null, 'path'=>null);

		return NLAbstractHTMLForm::BuildImage($k, $value);
	}


	function buildCell($field, $object, $param=NULL) {
		return $this->buildCellHtml($object[$field->name]);
	}

	function buildCellHtml($value) {
		$html  = '<div class="imagepreview image-in-cell"><div class="small">';

		if(isset($value['path'])) {
			$size = 50;

			$w = isset($value['width'])  ? $value['width']  : $size;
			$h = isset($value['height']) ? $value['height'] : $size;

			if($w > $size || $h > $size) {
				$width  = ($w - $h) > 0 ? $size : floor($size * $w/$h);
				$height = ($w - $h) > 0 ? floor($size * $h/$w) : $size;
			} else {
				$width  = $w;
				$height = $h;
			}

			$html .= '<a href="'.$value['path'].'" target="_blank"><img src="'.$value['path'].'" width="'.$width.'" height="'.$height.'"/></a>';
		}

		$html .= '</div>'.(isset($value['path']) ? $value['width'].'x'.$value['height'] : '').'</div>';
		return $html ;
	}

	function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetImageVar($field->name);
	}
}
?>