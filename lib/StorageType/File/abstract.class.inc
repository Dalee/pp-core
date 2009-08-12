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

	function cloneFile($field, $object, &$param) {
		$donor = $param['donor'];

		$fileArray       = $donor[$field->name];
		$ai              = $this->ai($param['format'], $object['id'], $field->name);
		$basedir         = BASEPATH.'/site/htdocs'.$ai;

		if (is_array($fileArray)) {
			MakeDirIfNotExists($basedir);

			$object[$field->name]['path'] = $ai.basename($fileArray['path']);

			copy(
				BASEPATH.'/site/htdocs'.$donor[$field->name]['path'],
				BASEPATH.'/site/htdocs'.$object[$field->name]['path']
			);
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
	
	function ai($format, $id, $name){
		return '/ai/'.$format.'/'.$id.'/'.$name.'/';
	}
	
	function normalize($field, $object, &$param, $allowed = null) {
		$key = $this->ai($param['format'],$object['id'],$field->name);
		if(is_array($fileInfo = $this->getFromCache($key))){
			return reset($fileInfo);
		}

		return $this->saveToCache($key, $this->__normalize($field, $object, $param, $allowed));
	}

	function __normalize(){
		FatalError(get_class($this)."::__normalize() doesn't exists !");
	}

	function notInDb($field, $object, $param=NULL) {
		return true;
	}
}
?>
