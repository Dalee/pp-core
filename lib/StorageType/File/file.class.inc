<?php

/**
 * Файл
 * хранится в site/htdocs/ai/ТИП_ДАННЫХ/ИД_ОБЪЕКТА/ИМЯ_ПОЛЯ/ИМЯ_ФАЙЛА
 * например: site/htdocs/ai/news/123034/attach/rules.doc
 *
 * displayType FILE
 * storageType file
 */
class PXStorageTypeFile extends PXStorageTypeAbstractFile
{

	/**
	* {@inheritdoc}
	*/
	public function proceedFile(PXFieldDescription $field, array &$object, ?array &$param): ?array
	{
		$fileArray = &$object[$field->name];
		$ai = $this->ai($field, $object, $param['format']);
		$basedir = HTDOCS_PATH . $ai;
		$invalidateCache = false;

		if (is_array($fileArray)) {

			// добавление нового файла, либо загрузка нового
			if (!empty($fileArray['name'])) {
				$filename = null;

				if (!empty($param['allowed']) && is_array($param['allowed'])) {
					$allowed = $param['allowed'];

					$tmp = $this->normalizeFileName($fileArray['name']);
					$tmp = preg_replace('/\.\w+$/' . REGEX_MOD, '', $tmp);

					if (in_array($fileArray['type'], array_keys($allowed))) {
						$filename = $tmp . '.' . $allowed[$fileArray['type']];
					}

				} else {
					$filename = $this->normalizeFileName($fileArray['name']);
				}

				// ModifyContent: если мы загружаем новый файл, мы трем все файлы
				// в директории, и копируем туда новый
				if ($filename !== null) {
					$d = new NLDir($basedir);
					if (!$d->Exist()) {
						$d->Create();
					}
					$d->Emptyfy(false);

					$this->copyFile($field, $fileArray['tmp_name'], $basedir . $filename);
					$invalidateCache = true;
				}
			}

			// запрос на удаление файла/файлов
			if (isset($fileArray['remove']) && is_array($fileArray['remove'])) {

				foreach ($fileArray['remove'] as $filename) {
					// удаление файла на файловой системе, если он есть
					if (file_exists($basedir . $filename)) {
						unlink($basedir . $filename);
						$invalidateCache = true;
					}

					// удаление файла из sys_meta, если есть ссылка на файл
					$this->removeDataFromMeta(
						$field,
						$object,
						$param,
						$filename
					);
				}
			}
		}

		if ($invalidateCache) {
			unlinkDirTreeIfEmpty($basedir);
		}

		// пишем в объект общий результат нормализации
		// включая поля tmp_name, error_code, mime_type, etc..
		// необходимо для триггеров
		$fileArray = $this->normalize($field, $object, $param);

		// в функцию добавления/обновления объекта возвращаем
		// только конкретную нормализацию файлового поля объекта
		// без временных полей.
		$normalized = $this->__normalize($field, $object, $param);
		$normalized = is_array($normalized) ? array_filter($normalized) : null;

		return $normalized;
	}

	/**
	 * {@inheritdoc}
	 */
	public function __normalize($field, $object, &$param)
	{

		$fileName = null;
		$relativeDir = $this->ai($field, $object, $param['format']);
		$fullDir = path_clear(HTDOCS_PATH . $relativeDir);
		$storageTypeDir = new NLDir($fullDir);

		if ($storageTypeDir->Exist()) {
			while ($entry = $storageTypeDir->Read()) {
				$fileName = $entry;
			}
		}

		//
		// файл присутствует на диске, обновляем данные по файлу
		//
		if ($fileName !== null) {
			$fullPath = path_clear($fullDir . DIRECTORY_SEPARATOR . $fileName);
			$relativePath = path_clear($relativeDir . DIRECTORY_SEPARATOR . $fileName);
			$mimeType = null;
			$fileSize = 0;

			if (is_file($fullPath)) {
				$mimeType = mime_content_type($fullPath);
				$fileSize = filesize($fullPath);
			}

			return [
				'filename' => $fileName,
				'path' => $relativePath,
				'fullpath' => $relativePath,
				'uri' => $relativePath, // URI for SecureFile
				'type' => $mimeType,
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
