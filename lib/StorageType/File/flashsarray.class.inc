<?php
/**
 * ������ ������
 * �������� � site/htdocs/ai/���_������/��_�������/���_����/�����.swf
 * ��������: site/htdocs/ai/postcard/123034/movies/edgar.swf
 *           site/htdocs/ai/postcard/123034/movies/banner2.swf
 *           site/htdocs/ai/postcard/123034/movies/3.swf
 *
 * displayType FLASHSARRAY
 * storageType flashsarray
 */
class PXStorageTypeFlashsarray extends PXStorageTypeImagesarray {
	static function proceedFile($field, $object, &$param) {
		return PXStorageTypeImagesarray::proceedFile($field, $object, $param, ENUM_ALLOWED_FLASHTYPES);
	}
}
?>