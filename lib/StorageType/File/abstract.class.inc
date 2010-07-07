<?php
/**
 * Базовый класс для работы с файлами
 */
class PXStorageTypeAbstractFile extends PXStorageType {
	function &_getCache(){
		static $aiCache;
		$pid = getmypid(); //cronjob protection
		if(!isset($aiCache[$pid])){
			$db = & PXRegistry::getDb(); //FIXME: possibly unsafe DB link getter
			$cacheType     = isset($db->selfDescription->aicache) ? $db->selfDescription->aicache : 'null';
			$aiCache[$pid] = ObjectCache::get($cacheType, 'ai', 24 * 60 * 60);
		}
		return $aiCache[$pid];
	}

	function cloneFile($field, $object, &$param, $symlink = false) {
		$donor = $param['donor'];
		$fileArray       = $donor[$field->name];
		$ai              = $this->ai($field, $object, $param['format']);
		$basedir         = HTDOCS_PATH . $ai;

		if (is_array($fileArray) && !empty($donor[$field->name]['path'])) {
			MakeDirIfNotExists($basedir);
			if($symlink) {
				symlink(
					HTDOCS_PATH . $donor[$field->name]['path'],
					HTDOCS_PATH . $ai.basename($fileArray['path'])
				);
			} else {
				copy(
					HTDOCS_PATH . $donor[$field->name]['path'],
					HTDOCS_PATH . $ai.basename($fileArray['path'])
				);
			}
		}
	}

	function getFromCache($key){
		$cache = & PXStorageTypeAbstractFile::_getCache();
		return $cache->load($key);
	}
	
	function removeFromCache($key){
		$cache = & PXStorageTypeAbstractFile::_getCache();
		$cache->delete($key);
	}
	
	function saveToCache($key, $data){
		$cache = & PXStorageTypeAbstractFile::_getCache();
		$cache->save($key, array($data));
		return $data;
	}
	
	function ai($field, &$object, $format){
		$params = array(
			'format' => $format,
			'id'     => $object['id'],
			'name'   => $field->name
		);

		foreach(PXRegistry::getApp()->triggers->{'storagetype'} as $t) {
			$t->getTrigger()->normalizeFilePath($field, $object, $params);
		}
		return '/ai/' . implode('/', $params) . '/';
	}
	
	function normalize($field, $object, &$param, $allowed = null) {
		$key = $this->ai($field, $object, $param['format']);
		if(is_array($fileInfo = $this->getFromCache($key))){
			return reset($fileInfo);
		}

		$normalizedObject = $this->saveToCache($key, $this->__normalize($field, $object, $param, $allowed));

		//For some triggers, which need original fields like tmp_name, etc ...
		if (isset($object[$field->name]) && is_array($object[$field->name]) && is_array($normalizedObject)) {
			$normalizedObject = array_merge($object[$field->name], $normalizedObject);
		}

		return $normalizedObject;
	}

	function __normalize(){
		FatalError(get_class($this)."::__normalize() doesn't exists !");
	}

	function notInDb($field, $object, $param=NULL) {
		return true;
	}
	
	function copyFile($field, $src, $dest) {
		if (count($field->storageTypeArgs) > 1) {
			
			$width  = $field->storageTypeArgs[0];
			$height = $field->storageTypeArgs[1];
			GDImageResize($src, $dest, $width, $height);
			
		} else {
			copy($src, $dest);
		}
	}
}
?>
