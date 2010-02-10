<?php
/**
 * σΠΙΣΟΛ ΖΜάΫΕΚ
 * ΘÒΑΞΡΤΣΡ Χ site/htdocs/ai/τιπ_δαξξωθ/ιδ_οβÿελτα/ινρ_πομρ/ξονες.swf
 * ΞΑΠÒΙΝΕÒ: site/htdocs/ai/postcard/123034/movies/edgar.swf
 *           site/htdocs/ai/postcard/123034/movies/banner2.swf
 *           site/htdocs/ai/postcard/123034/movies/3.swf
 *
 * displayType FLASHSARRAY
 * storageType flashsarray
 */
class PXStorageTypeFlashsarray extends PXStorageTypeImagesarray {
	function proceedFile($field, &$object, &$param) {
		return parent::proceedFile($field, $object, $param, ENUM_ALLOWED_FLASHTYPES);
	}
}
?>