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

	/**
	 * {@inheritdoc}
	 */
	function proceedFile($field, &$object, &$param, $allowed = ENUM_ALLOWED_IMAGETYPES) {
		$param['allowed'] = unserialize($allowed); //PHP54: $allowed is for arguments contract at now
		return parent::proceedFile($field, $object, $param, $allowed);
	}

	/**
	 * {@inheritdoc}
	 */
	function __normalize($field, $object, &$param, $allowed = null) {

		$relativeDir = $this->ai($field, $object, $param['format']);
		$fullDir = HTDOCS_PATH . $relativeDir;
		$storageTypeDir = new NLDir($fullDir);

		//
		// директория для типа присутствует на диске,
		// обновляем данные по файлу.
		//
		if ($storageTypeDir->Exist()) {
			$entriesList = [];

			foreach ($storageTypeDir->getFileNames() as $entry) {
				$fullPath = path_clear($fullDir.$entry);
				$relativePath = path_clear($relativeDir.DIRECTORY_SEPARATOR.$entry);

				$width = 0;
				$height = 0;
				$size = 0;
				if (is_file($fullPath)) {
					list($width, $height,) = getimagesize($fullPath);
					$size = filesize($fullPath);
				}

				$entriesList[$entry] = [
					'entry' => $entry,
					'path' => $relativePath,
					'width' => $width,
					'height' => $height,
					'size' => $size,
				];
			}

			return $entriesList;
		}

		//
		// файл отсутствует на диске
		// @see getDataFromMeta
		//
		return $this->getDataFromMeta($field, $object, $param);
	}
}
