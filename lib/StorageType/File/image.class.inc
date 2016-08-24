<?php
/**
 * Картинка
 * хранится в site/htdocs/ai/ТИП_ДАННЫХ/ИД_ОБЪЕКТА/ИМЯ_ПОЛЯ/ИМЯ_ФАЙЛА.РАЗРЕШЕННЫЙ_ТИП
 * например: site/htdocs/ai/news/123034/image/bush.gif
 *
 * displayType IMAGE
 * storageType image
 */
class PXStorageTypeImage extends PXStorageTypeFile {
	//FIXME: Remove static usage of this method from projects !
	function proceedFile($field, &$object, &$param, $allowed=ENUM_ALLOWED_IMAGETYPES) {
		$param['allowed'] = unserialize($allowed); //PHP54: $allowed is for arguments contract at now
		return parent::proceedFile($field, $object, $param, $allowed);
	}

	function __normalize($field, $object, &$param, $allowed = null) {
		$allowedImageTypes = array_unique(isset($param['allowed']) ? $param['allowed'] : unserialize(ENUM_ALLOWED_IMAGETYPES));
		$basedir           = $this->ai($field, $object, $param['format']);
		$tmp               = glob(HTDOCS_PATH . $basedir . '/*');

		if (!count($tmp)) {
			return;
		}

		if (!in_array(mb_strtolower(mb_substr($tmp[0], -3)), $allowedImageTypes)) {
			return;
		}

		$diskPath = $tmp[0];
		$httpPath = str_replace(HTDOCS_PATH, '', $diskPath);
		$httpPath = urlencode($httpPath);
		$httpPath = str_replace('%2F', '/', $httpPath);
		$httpPath = str_replace('//', '/', $httpPath);
		$httpPath = str_replace('+', '%20', $httpPath);

		list($width, $height, ) = getimagesize($diskPath);

		return [
			'path'   => isset($httpPath) ? $httpPath : NULL,
			'width'  => isset($width)    ? $width    : NULL,
			'height' => isset($height)   ? $height   : NULL,
			'size'   => is_file($diskPath) ? filesize($diskPath) : 0,
		];
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

