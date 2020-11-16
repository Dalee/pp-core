<?php

/**
 * Список файлов
 * хранятся в htdocs/ai/ТИП_ДАННЫХ/ИД_ОБЪЕКТА/ИМЯ_ПОЛЯ/ИМЯ_ФАЙЛА
 * например: htdocs/ai/news/123034/attach/rules.doc
 *           htdocs/ai/news/123034/attach/cases.xls
 *           htdocs/ai/news/123034/attach/nude.jpg
 *
 * displayType FILESARRAY
 * storageType filesarray
 */
class PXStorageTypeFilesarray extends PXStorageTypeAbstractFile
{

	/**
	 * {@inheritdoc}
	 */
	public function cloneFile($field, $object, &$param, $symlink = false)
	{

		$donor = $param['donor'];
		$fileArray = $donor[$field->name];

		$ai = $this->ai($field, $object, $param['format']);
		$basedir = HTDOCS_PATH . $ai;

		if (!is_array($fileArray) || empty($fileArray)) {
			return;
		}

		MakeDirIfNotExists($basedir);

		foreach ($fileArray as $k => $f) {
			$object[$field->name][$k]['path'] = $ai . $f['entry'];

			if ($symlink) {
				symlink(
					HTDOCS_PATH . $donor[$field->name][$k]['path'],
					HTDOCS_PATH . $object[$field->name][$k]['path']
				);

			} else {
				copy(
					HTDOCS_PATH . $donor[$field->name][$k]['path'],
					HTDOCS_PATH . $object[$field->name][$k]['path']
				);
			}
		}
	}

	/**
	 * {@inheritdoc}
	 * @return array|null
	 */
	public function proceedFile(PXFieldDescription $field, array &$object, ?array &$param): ?array
	{
		$varName = $field->name;
		$ai = $this->ai($field, $object, $param['format']);
		$basedir = HTDOCS_PATH . $ai;
		$fileArray = &$object[$varName];
		$invalidateCache = false;
		$allowed = $param['allowed'] ?? null;

		$copyFilesFun = function(&$fileArray, bool $replaceOldFiles = false) use ($allowed, $basedir, $field, &$invalidateCache) {
			if (!is_array($fileArray['name'] ?? null)) {
				return;
			}
			foreach ($fileArray['name'] as $oldFileNameOrIndex => $newFileName) {
				$filename = null;

				// if no filter by extension
				if (empty($allowed) || !is_array($allowed)) {
					$filename = $basedir . DIRECTORY_SEPARATOR . $this->normalizeFileName($newFileName);

					// if type in allowed list
				} elseif (in_array($fileArray['type'][$oldFileNameOrIndex], array_keys($allowed))) {
					$tmp = $this->normalizeFileName($newFileName);
					$tmp = preg_replace('/\.\w+$/' . REGEX_MOD, '', $tmp);

					if (in_array($fileArray['type'][$oldFileNameOrIndex], array_keys($allowed))) {
						$filename = $basedir . DIRECTORY_SEPARATOR . $tmp . '.' . $allowed[$fileArray['type'][$oldFileNameOrIndex]];
					}
				}

				if (!is_null($filename) &&
					(!$replaceOldFiles || file_exists($basedir . DIRECTORY_SEPARATOR . $oldFileNameOrIndex))) {
					MakeDirIfNotExists($basedir);
					$replaceOldFiles && unlink($basedir . DIRECTORY_SEPARATOR . $oldFileNameOrIndex);

					$this->copyFile($field, $fileArray['tmp_name'][$oldFileNameOrIndex], $filename);
					$invalidateCache = true;
				}
			}
		};

		// add new files
		$copyFilesFun($fileArray);
		/** replace existing files @see PXDisplayTypeFilesarray::getFromRequest() */
		isset($fileArray['edit']) && $copyFilesFun($fileArray['edit'], true);
		// remove deleted files
		if (is_array($fileArray['remove'] ?? null)) {
			$filename = null;

			foreach ($fileArray['remove'] as $filename) {

				if (file_exists($basedir . DIRECTORY_SEPARATOR . $filename)) {
					unlink($basedir . DIRECTORY_SEPARATOR . $filename);
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
		$normalized = is_array($normalized) ? array_filter($normalized) : [];

		return count($normalized) ? $normalized : null;
	}

	/**
	 * {@inheritdoc}
	 */
	public function __normalize($field, $object, &$param)
	{
		$basedir = $this->ai($field, $object, $param['format']);
		$storageTypeDir = new NLDir(HTDOCS_PATH . $basedir);

		//
		// директория присутствует на диске,
		// обновляем данные по файлу.
		//
		if ($storageTypeDir->Exist()) {
			$filesArray = [];
			$fileEntries = $storageTypeDir->getFileNames();

			foreach ($fileEntries as $entry) {
				$diskPath = HTDOCS_PATH . $basedir . $entry;
				$httpPath = $basedir . $entry;

				$filesArray[$entry] = [
					'path' => $httpPath ?? null,
					'uri' => $httpPath ?? null, // same as in PXStorageTypeFile, for SecureFile
					'filename' => $entry,
					'size' => filesize($diskPath),
					'type' => mime_content_type($diskPath),
					'entry' => $entry,
				];
				$filesArray[$entry]['fullpath'] = &$filesArray[$entry]['path'];
			}

			return $filesArray;
		}

		//
		// файл отсутствует на диске
		// @see getDataFromMeta
		//
		return $this->getDataFromMeta($field, $object, $param);
	}
}
