<?php
class PXTriggerDatabaseAbstractAdvert extends PXAbstractDatabaseTrigger {
	var $dtPlace;
	var $dtBanner;
	var $refName;
	var $db;
	var $isDynamic;
	var $imageFields = array('image', 'flash');

	function PXTriggerDatabaseAbstractAdvert() {
		$this->path       = BASEPATH.'site/var/ad/in';
		$this->db         =& PXRegistry::getDb();
		$settings         = $this->db->app->modules['advert']->settings;
		
		$this->dtBanner   = $this->db->app->types[$settings['banner']];
		$this->dtPlace    = $this->db->app->types[$settings['place']];
		$this->refName    = $this->db->app->references[$settings['banner'].'2'.$settings['place']];
	}

	function OnAddObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues) {
		if ($format->id == 'adplace' || $format->id == 'adbanner') {
			$this->adSync($format->id, $object);
		}
	}

	function OnDeleteObject(&$db, &$objId, &$format, &$object) {
		if ($format->id == 'adplace' || $format->id == 'adbanner') {
			$name = ($format->id == 'adplace') ? "/adv_p" : "/adv_b";
			$this->_writeCache($this->path.$name.$object['id'].".dat", null, false);
		}
	}

	function OnModifyObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues) {
		if ($format->id == 'adplace' || $format->id == 'adbanner') {
			$this->adSync($format->id, $object);
		}
	}

	function OnModifyLinks(&$db, &$objId, &$formatId) {
		if ($formatId == 'adplace' || $formatId == 'adbanner') {
			$object = $this->db->GetObjectById($this->db->app->types[$formatId], $objId);
			$this->adSync($formatId, $object);
		}
	}

	function writeBanner($object) {
		return array();
	}

	function normalizeFiles(&$object) {
		$emptyArray = array (
			'path'   => NULL,
			'width'  => NULL,
			'height' => NULL
		);
		
		foreach ($this->imageFields as $imgField) {
			if (isset($object[$imgField])) {
				$object[$imgField] = (array)$object[$imgField] + $emptyArray;
			}
		}
	}

	protected function getBannersById($id) {
		return $this->db->GetObjectsByIdArray($this->dtBanner, TRUE, array_keys($this->db->GetLinks($this->refName, $this->dtPlace->id, $id)));
	}

	protected function getPlacesById($id) {
		return $this->db->GetObjectsByIdArray($this->dtPlace, TRUE, array_keys($this->db->GetLinks($this->refName, $this->dtBanner->id, $id)));
	}

	function adbanner($object) {
		$name = $this->path."/adv_b".$object['id'].".dat";

		$this->normalizeFiles($object);
		$this->_writeCache($name, implode("\n", $this->writeBanner($object)) . "\n", $object['status']);

		$places  = $this->getPlacesById($object['id']);

		foreach ($places as $place) {
			$this->adplace($place);
		}
	}

	function adplace($object) {
		$name =$this->path."/adv_p".$object['id'].".dat";
		$text = $object['id']."\n";
		$text .= $object['type']."\n";

		$attached = $this->getBannersById($object['id']);

		foreach ($attached as $k=>$v) {
			if($v['status']) {
				$text .= $k."\n";
			}
		}

		$this->_writeCache($name, $text, $object['status']);
	}

	function _writeCache($name, $text, $status) {
		if(!$status){
			$handle = opendir($this->path);

			if (!$handle) {
				return;
			}

			@unlink($name);
			closedir($handle);
		} else {
			$fd = fopen($name, "w");
			fwrite($fd, $text);
			fclose($fd);
		}

	}
	
	function _getFormat($object) {
		return (empty($object['flash']['path'])) ? $object['type']."_gif" : $object['type']."_flash";
	}

	function adSync($format, $object) {
		if($object != null) {
			if (isset($object['dynamic'])) {
				$this->isDynamic = $object['dynamic'] ? 1 : 0;
			}
			
			if (!isset($object['reference']) || !trim($object['reference'])) {
				$object['reference'] = '#';
			}
			
			$fd = fopen($this->path."/reload", "r");
			flock($fd, LOCK_EX);

			$this->$format($object);

			flock($fd, LOCK_UN);
			fclose($fd);

			touch($this->path.'/reload');
			sleep(1);
		}
	}
}
?>
