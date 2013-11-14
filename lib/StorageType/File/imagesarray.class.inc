<?php
/**
 * Список картинок
 * хранятся в site/htdocs/ai/ТИП_ДАННЫХ/ИД_ОБЪЕКТА/ИМЯ_ПОЛЯ/ИМЯ_ФАЙЛА.РАЗРЕШЕННЫЙ_ТИП
 * например: site/htdocs/ai/news/123034/images/1.jpg
 *           site/htdocs/ai/news/123034/images/girl.png
 *           site/htdocs/ai/news/123034/images/dos.jpg
 *
 * displayType IMAGESARRAY
 * storageType imagesarray
 */
class PXStorageTypeImagesarray extends PXStorageTypeFilesarray {
	//FIXME: Remove possible static usage of this method from projects !
	function proceedFile($field, &$object, &$param, $allowed=ENUM_ALLOWED_IMAGETYPES) {
		$param['allowed'] = unserialize($allowed); //PHP54: $allowed is for arguments contract at now
		return parent::proceedFile($field, $object, $param, $allowed);
	}

	function __normalize($field, $object, &$param, $allowed = null) {
		$imagesArray = array();
		$basedir     = $this->ai($field, $object, $param['format']);;

		$d = new NLDir(HTDOCS_PATH . $basedir);

		if ($d->Exist()) {
			foreach ($d->getFileNames() as $entry) {
				$diskPath = HTDOCS_PATH . $basedir . $entry;
				$httpPath = $basedir.$entry;
				list($width, $height, ) = getimagesize($diskPath);

				$imagesArray[$entry] = array(
					'path'   => isset($httpPath) ? $httpPath : NULL,
					'width'  => isset($width)    ? $width    : NULL,
					'height' => isset($height)   ? $height   : NULL,
					'entry'  => $entry
				);

				unset($httpPath);
				unset($width);
				unset($height);
			}
		}

		return $imagesArray;
	}
}
?>
