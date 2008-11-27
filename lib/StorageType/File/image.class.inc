<?php
/**
 * ��������
 * �������� � site/htdocs/ai/���_������/��_�������/���_����/���_�����.�����������_���
 * ��������: site/htdocs/ai/news/123034/image/bush.gif
 *
 * displayType IMAGE
 * storageType image
 */
class PXStorageTypeImage extends PXStorageTypeFile {
	static function proceedFile($field, $object, &$param, $allowed=ENUM_ALLOWED_IMAGETYPES) {
		$param['allowed'] = unserialize($allowed);
		return PXStorageTypeFile::proceedFile($field, $object, $param);
	}

	static function normalize($field, $object, &$param, $allowed=ENUM_ALLOWED_IMAGETYPES) {
		$allowedImageTypes = array_unique(unserialize($allowed));

		$tmp = glob(BASEPATH.'/site/htdocs/ai/'.$param['format'].'/'.$object['id'].'/'.$field->name.'/*');

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