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
	//FIXME: Remove static usage of this method from projects !
	function proceedFile($field, &$object, &$param, $allowed=ENUM_ALLOWED_IMAGETYPES) {
		$param['allowed'] = unserialize($allowed);
		return parent::proceedFile($field, $object, $param);
	}

	function __normalize($field, $object, &$param, $allowed) {
		$allowedImageTypes = array_unique(unserialize(is_null($allowed) ? ENUM_ALLOWED_IMAGETYPES : $allowed));
		$basedir           = $this->ai($field, $object, $param['format']);
		$tmp               = glob(HTDOCS_PATH . $basedir . '/*');

		if (!count($tmp)) {
			return;
		}

		if (!in_array(strtolower(substr($tmp[0], -3)), $allowedImageTypes)) {
			return;
		}

		$diskPath = $tmp[0];
		$httpPath = str_replace(HTDOCS_PATH, '', $diskPath);
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

	function copyFile($field, $src, $dest) {
		if (count($field->storageTypeArgs) == 2) { //REMOVEME: This is obsoleted and deprecated feature !
			$width  = $field->storageTypeArgs[0];
			$height = $field->storageTypeArgs[1];
			GDImageResize($src, $dest, $width, $height);
		} else {
			parent::copyFile($field, $src, $dest);
		}
	}
}
?>
