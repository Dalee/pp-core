<?php

use PP\Lib\Cache\ObjectCache;

/**
 * Базовый класс для работы с файлами
 */
class PXStorageTypeAbstractFile extends PXStorageType
{

	public function _getCache()
	{ //REMOVEME: Obsoleted and unused feature.
		static $aiCache;

		$pid = getmypid(); //cronjob protection
		if (!isset($aiCache[$pid])) {
			$db = PXRegistry::getDb(); //FIXME: possibly unsafe DB link getter

			// TODO: property accessor
			$cacheType = isset($db->getSelfDescription()->aicache)
				? $db->getSelfDescription()->aicache
				: 'null';

			$aiCache[$pid] = ObjectCache::get($cacheType, 'ai', 24 * 60 * 60);
		}

		return $aiCache[$pid];
	}

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
					HTDOCS_PATH . $ai . basename($fileArray['path'])
				);
			} else {
				copy(
					HTDOCS_PATH . $donor[$field->name]['path'],
					HTDOCS_PATH . $ai . basename($fileArray['path'])
				);
			}
		}
	}

	public function getFromCache($key)
	{
		return self::_getCache()->load($key);
	}

	public function removeFromCache($key)
	{
		self::_getCache()->delete($key);
	}

	public function saveToCache($key, $data)
	{
		self::_getCache()->save($key, [$data]);
		return $data;
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

	public function normalize($field, $object, &$param, $allowed = null)
	{
		$key = $this->ai($field, $object, $param['format']);
		if (is_array($fileInfo = $this->getFromCache($key))) {
			return reset($fileInfo);
		}

		$normalizedObject = $this->saveToCache($key, $this->__normalize($field, $object, $param, $allowed));

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
	 * @param null|array $allowed
	 * @return array
	 */
	public function __normalize($field, $object, &$param, $allowed = null)
	{
		FatalError(get_class($this) . "::__normalize() doesn't exists !");
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
		$sysMeta = (isset($param[OBJ_FIELD_META]))
			? $param[OBJ_FIELD_META]
			: [];

		return (isset($sysMeta[$field->name]))
			? $sysMeta[$field->name]
			: null;
	}

	/**
	 * Удаление записи о файле из sys_meta массива
	 *
	 * @param PXFieldDescription $field
	 * @param array $object
	 * @param array $param
	 * @param string $fileName
	 *
	 * @return void
	 */
	protected function removeDataFromMeta($field, $object, &$param, $fileName)
	{
		$sysMeta = (isset($param[OBJ_FIELD_META]))
			? $param[OBJ_FIELD_META]
			: [];

		$fileData = (isset($sysMeta[$field->name]))
			? $sysMeta[$field->name]
			: null;

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
			$entry = basename($metaPart['path']);
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
}
