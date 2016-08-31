<?php
/**
 * Список флэшей
 * хранятся в site/htdocs/ai/ТИП_ДАННЫХ/ИД_ОБЪЕКТА/ИМЯ_ПОЛЯ/НОМЕР.swf
 * например: site/htdocs/ai/postcard/123034/movies/edgar.swf
 *           site/htdocs/ai/postcard/123034/movies/banner2.swf
 *           site/htdocs/ai/postcard/123034/movies/3.swf
 *
 * displayType FLASHSARRAY
 * storageType flashsarray
 */
class PXStorageTypeFlashsarray extends PXStorageTypeImagesarray {

	/**
	 * {@inheritdoc}
	 */
	function proceedFile($field, &$object, &$param, $allowed = ENUM_ALLOWED_FLASHTYPES) {
		return parent::proceedFile($field, $object, $param, $allowed);
	}
}
