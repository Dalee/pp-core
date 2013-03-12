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
	function proceedFile($field, &$object, &$param, $allowed = ENUM_ALLOWED_FLASHTYPES) {
		return parent::proceedFile($field, $object, $param, $allowed);
	}

	function normalize($field, $object, &$param, $allowed = ENUM_ALLOWED_FLASHTYPES) {
		return parent::normalize($field, $object, $param, $allowed);
	}
}
?>
