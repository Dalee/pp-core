<?php

/**
 * Базовый класс для работы с файлами
 */
class PXStorageTypeAbstractFile extends PXStorageType
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

		if (is_array($fileArray) && !empty($donor[$field->name]['path'])) {
			MakeDirIfNotExists($basedir);
			if ($symlink) {
				symlink(
					HTDOCS_PATH . $donor[$field->name]['path'],
					HTDOCS_PATH . $ai . basename((string) $fileArray['path'])
				);
			} else {
				copy(
					HTDOCS_PATH . $donor[$field->name]['path'],
					HTDOCS_PATH . $ai . basename((string) $fileArray['path'])
				);
			}
		}
	}

	public function ai($field, &$object, $format)
	{
		$params = [
			'format' => $format,
			'id' => $object['id'],
			'name' => $field->name,
		];

		foreach (PXRegistry::getApp()->triggers->{'storagetype'} as $t) {
			$t->getTrigger()->normalizeFilePath($field, $object, $params);
		}

		return '/ai/' . implode('/', $params) . '/';
	}

	public function normalize($field, $object, &$param)
	{
		$normalizedObject = $this->__normalize($field, $object, $param);

		//For some triggers, which need original fields like tmp_name, etc ...
		if (isset($object[$field->name]) && is_array($object[$field->name]) && is_array($normalizedObject)) {
			$normalizedObject = array_merge($object[$field->name], $normalizedObject);
		}

		return $normalizedObject;
	}

	/**
	 * @param PXFieldDescription $field
	 * @param array $object
	 * @param array $param
	 * @return array|null
	 */
	public function __normalize($field, $object, &$param)
	{
		FatalError(static::class . "::__normalize() doesn't exists !");
	}

	public function storedInDb()
	{
		return false;
	}

	public function copyFile($field, $src, $dest)
	{
		copy($src, $dest);
	}

	/**
	 * Получение данных из sys_meta
	 *
	 * @param PXFieldDescription $field
	 * @param array $object
	 * @param array $param
	 *
	 * @return array|null
	 */
	protected function getDataFromMeta($field, $object, &$param)
	{
		//
		// файл отсутствует на диске, попытаемся вытащить данные из sys_meta
		// в случае если был запрос на удаление данных, данные о файле уже
		// должны быть удалены из sys_meta
		// Данное поведение обязательно при активированном плагине загрузки
		// файлов в s3 (ceph) облако.
		//
		$sysMeta = $param[OBJ_FIELD_META] ?? [];

		return $sysMeta[$field->name] ?? null;
	}

	/**
  * Удаление записи о файле из sys_meta массива
  *
  *
  * @return void
  */
 protected function removeDataFromMeta(PXFieldDescription $field, array $object, array &$param, string $fileName)
	{
		$sysMeta = $param[OBJ_FIELD_META] ?? [];

		$fileData = $sysMeta[$field->name] ?? null;

		if ($fileData === null) {
			return;
		}

		//
		// данные по файлу могут быть упакованы как с однарной вложенностью,
		// так и с двойной (например в случае imagesarray)
		//
		$sysMeta[$field->name] = $this->recursiveMetaEntryDelete(
			$fileData,
			$fileName
		);

		// Обновляем sys_meta в param.
		$param[OBJ_FIELD_META] = $sysMeta;
	}

	/**
	 * Рекурсивная часть функции removeDataFromMeta
	 *
	 * @param $metaPart
	 * @param $fileName
	 * @return array|null
	 */
	protected function recursiveMetaEntryDelete($metaPart, $fileName)
	{
		if (!is_array($metaPart)) {
			return $metaPart;
		}

		if (isset($metaPart['path'])) {
			$entry = basename((string) $metaPart['path']);
			if ($entry === $fileName) {
				return null;
			}
		} else {
			foreach ($metaPart as $key => $val) {
				$metaPart[$key] = $this->recursiveMetaEntryDelete($val, $fileName);
			}
		}

		return $metaPart;
	}

	protected function normalizeFileName(string $fileName): string
	{
		return match ($this->field->storageTypeArgs[0] ?? null) {
			'filter' => path_filter_filename($fileName),
			default => _TranslitFilename($fileName),
		};
	}
}
