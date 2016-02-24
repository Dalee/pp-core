<?php
/**
 * Картинка
 *
 * storageType image
 * displayType IMAGESTATIC
 */
class PXDisplayTypeImageStatic extends PXDisplayTypeImage {
	function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = (isset($object[$k]) && is_array($object[$k]) && isset($object[$k]['path']) && !is_null($object[$k]['path']))
			? $object[$k]
			: array('width'=>null, 'height'=>null, 'path'=>null, 'size'=>null);

		$image = new PXInputImageStatic($k, $value);
		return $image->html();
	}
}
?>
