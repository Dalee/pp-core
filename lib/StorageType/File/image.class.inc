<?php
/**
 * ëÁÒÔÉÎËÁ
 * ÈÒÁÎÉÔÓÑ × site/htdocs/ai/ôéð_äáîîùè/éä_ïâÿåëôá/éíñ_ðïìñ/éíñ_æáêìá.òáúòåûåîîùê_ôéð
 * ÎÁÐÒÉÍÅÒ: site/htdocs/ai/news/123034/image/bush.gif
 *
 * displayType IMAGE
 * storageType image
 */
class PXStorageTypeImage extends PXStorageTypeFile {
	function proceedFile($field, $object, &$param, $allowed=ENUM_ALLOWED_IMAGETYPES) {
		$param['allowed'] = unserialize($allowed);
		return parent::proceedFile($field, $object, $param);
	}

	function __normalize($field, $object, &$param, $allowed) {
		$allowedImageTypes = array_unique(unserialize(is_null($allowed) ? ENUM_ALLOWED_IMAGETYPES : $allowed));
		$basedir           = parent::ai($param['format'],$object['id'],$field->name);
		$tmp               = glob(BASEPATH.'/site/htdocs' . $basedir . '/*');

		if (!count($tmp)) {
			return;
		}

		if (!in_array(strtolower(substr($tmp[0], -3)), $allowedImageTypes)) {
			return;
		}

		$diskPath = $tmp[0];
		$httpPath = str_replace(BASEPATH.'/site/htdocs', '', $diskPath);
		$httpPath = urlencode($httpPath);
		$httpPath = str_replace('%2F', '/', $httpPath);
		$httpPath = str_replace('//', '/', $httpPath);
		$httpPath = str_replace('+', '%20', $httpPath);
		
		list($width, $height, ) = getimagesize($diskPath);

		return array(
			'path'   => isset($httpPath) ? $httpPath : NULL,
			'width'  => isset($width)    ? $width    : NULL,
			'height' => isset($height)   ? $height   : NULL
		);
	}
}
?>
