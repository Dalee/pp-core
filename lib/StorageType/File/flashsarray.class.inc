<?php
/**
 * óÐÉÓÏË ÆÌÜÛÅÊ
 * ÈÒÁÎÑÔÓÑ × site/htdocs/ai/ôéð_äáîîùè/éä_ïâÿåëôá/éíñ_ðïìñ/îïíåò.swf
 * ÎÁÐÒÉÍÅÒ: site/htdocs/ai/postcard/123034/movies/edgar.swf
 *           site/htdocs/ai/postcard/123034/movies/banner2.swf
 *           site/htdocs/ai/postcard/123034/movies/3.swf
 *
 * displayType FLASHSARRAY
 * storageType flashsarray
 */
class PXStorageTypeFlashsarray extends PXStorageTypeImagesarray {
	//FIXME: Remove possible static usage of this method from projects !
	function proceedFile($field, &$object, &$param, $allowed = ENUM_ALLOWED_FLASHTYPES) {
		return parent::proceedFile($field, $object, $param, $allowed);
	}
}
?>
