<?php
/**
 * Флешка
 * хранится в site/htdocs/ai/ТИП_ДАННЫХ/ИД_ОБЪЕКТА/ИМЯ_ПОЛЯ/ИМЯ_ФАЙЛА.swf
 * например: site/htdocs/ai/banner/125846/movie/banner.swf
 *
 * displayType FLASH
 * storageType flash
 */
class PXStorageTypeFlash extends PXStorageTypeImage {

	/**
	 * {@inheritdoc}
	 */
	function proceedFile($field, &$object, &$param, $allowed = ENUM_ALLOWED_FLASHTYPES) {
		return parent::proceedFile($field, $object, $param, $allowed);
	}

	/**
	 * {@inheritdoc}
	 */
	function normalize($field, $object, &$param, $allowed = ENUM_ALLOWED_FLASHTYPES) {
		return parent::normalize($field, $object, $param, $allowed);
	}
}
