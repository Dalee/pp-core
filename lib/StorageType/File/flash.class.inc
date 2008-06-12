<?php
/**
 * æÌÅÛËÁ
 * ÈÒÁÎÉÔÓÑ × site/htdocs/ai/ôéð_äáîîùè/éä_ïâÿåëôá/éíñ_ðïìñ/éíñ_æáêìá.swf
 * ÎÁÐÒÉÍÅÒ: site/htdocs/ai/banner/125846/movie/banner.swf
 *
 * displayType FLASH
 * storageType flash
 */
class PXStorageTypeFlash extends PXStorageTypeImage {
	static function proceedFile($field, $object, &$param) {
		return PXStorageTypeImage::proceedFile($field, $object, $param, ENUM_ALLOWED_FLASHTYPES);
	}

	static function normalize($field, $object, &$param) {
		return PXStorageTypeImage::normalize($field, $object, $param, ENUM_ALLOWED_FLASHTYPES);
	}
}
?>
