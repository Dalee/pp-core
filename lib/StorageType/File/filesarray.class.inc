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
class PXStorageTypeFilesarray extends PXStorageTypeAbstractFile {

	/**
	 * {@inheritdoc}
	 */
	function cloneFile($field, $object, &$param, $symlink = false) {

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
	 */
	function proceedFile($field, &$object, &$param, $allowed = null) {

		$varName = $field->name;
		$ai = $this->ai($field, $object, $param['format']);
		$basedir = HTDOCS_PATH . $ai;
		$fileArray = &$object[$varName];
		$allowed = isset($param['allowed']) ? $param['allowed'] : NULL;
		$invalidateCache = false;

		if (is_array($fileArray) && !empty($fileArray['name'][0])) {
			$filename = null;

			// if no filter by extension
			if (!is_array($allowed) || empty($allowed)) {
				$filename = $basedir . '/' . _TranslitFilename($fileArray['name'][0]);

				// if type in allowed list
			} elseif (in_array($fileArray['type'][0], array_keys($allowed))) {
				$tmp = _TranslitFilename($fileArray['name'][0]);
				$tmp = preg_replace('/\.\w+$/' . REGEX_MOD, '', $tmp);

				if (in_array($fileArray['type'][0], array_keys($allowed))) {
					$filename = $basedir . '/' . $tmp . '.' . $allowed[$fileArray['type'][0]];
				}
			}

			if (!is_null($filename)) {
				MakeDirIfNotExists($basedir);

				$this->copyFile($field, $fileArray['tmp_name'][0], $filename);
				$invalidateCache = true;
			}
		}

		if (is_array($fileArray) && isset($fileArray['remove']) && is_array($fileArray['remove'])) {
			$filename = null;

			foreach ($fileArray['remove'] as $filename) {

				if (file_exists($basedir . '/' . $filename)) {
					unlink($basedir . '/' . $filename);
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

		$edited = PXRegistry::getRequest()->GetUploadFile($varName . '_edit', true);
		if (isset($edited['name']) && is_array($edited['name'])) {
			foreach ($edited['name'] as $k => $v) {
				if (file_exists($basedir . '/' . $k) && isset($v) && strlen($v)) {
					unlink($basedir . '/' . $k);
					$this->copyFile($field, $edited['tmp_name'][$k], $basedir . '/' . _TranslitFilename($v));
					$invalidateCache = true;
				}
			}
		}

		if ($invalidateCache) {
			unlinkDirTreeIfEmpty($basedir);
			$this->removeFromCache($ai);
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
	function __normalize($field, $object, &$param, $allowed = null) {
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

				$filesArray[$entry] = array(
					'path' => isset($httpPath) ? $httpPath : NULL,
					'uri' => isset($httpPath) ? $httpPath : NULL, // same as in PXStorageTypeFile, for SecureFile
					'filename' => $entry,
					'size' => filesize($diskPath),
					'type' => mime_content_type($diskPath),
					'entry' => $entry
				);
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
