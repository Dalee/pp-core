<?php

/**
 * Картинка
 * хранится в site/htdocs/ai/ТИП_ДАННЫХ/ИД_ОБЪЕКТА/ИМЯ_ПОЛЯ/ИМЯ_ФАЙЛА.РАЗРЕШЕННЫЙ_ТИП
 * например: site/htdocs/ai/news/123034/image/bush.gif
 *
 * displayType IMAGE
 * storageType image
 */
class PXStorageTypeImage extends PXStorageTypeFile
{

	/**
  * {@inheritdoc}
  */
 public function proceedFile(PXFieldDescription $field, array &$object, ?array &$param): ?array
	{
		$param['allowed'] ??= ENUM_ALLOWED_IMAGETYPES;
		return parent::proceedFile($field, $object, $param);
	}

	/**
	 * {@inheritdoc}
	 */
	public function __normalize($field, $object, &$param)
	{
		$allowedImageTypes = $param['allowed'] ?? ENUM_ALLOWED_IMAGETYPES;
		$allowedImageTypes = array_unique($allowedImageTypes);

		$relativeDir = $this->ai($field, $object, $param['format']);
		$fullDir = path_clear(HTDOCS_PATH . DIRECTORY_SEPARATOR . $relativeDir);
		$storageTypeDir = new NLDir($fullDir);
		$fileName = null;

		if ($storageTypeDir->Exist()) {
			while ($entry = $storageTypeDir->Read()) {
				$fileName = $entry;
			}
		}

		//
		// файл присутствует на диске, обновляем данные по файлу
		//
		if ($fileName !== null) {
			if (!in_array(mb_strtolower(mb_substr($fileName, -3)), $allowedImageTypes)) {
				return null;
			}

			$fullPath = path_clear($fullDir . DIRECTORY_SEPARATOR . $fileName);
			$relativePath = path_clear($relativeDir . DIRECTORY_SEPARATOR . $fileName);

			$width = null;
			$height = null;
			$fileSize = 0;

			if (is_file($fullPath)) {
				[$width, $height,] = getimagesize($fullPath);
				$fileSize = filesize($fullPath);
			}

			return [
				'path' => $relativePath,
				'width' => $width,
				'height' => $height,
				'size' => $fileSize,
			];
		}

		//
		// файл отсутствует на диске
		// @see getDataFromMeta
		//
		return $this->getDataFromMeta($field, $object, $param);
	}
}
