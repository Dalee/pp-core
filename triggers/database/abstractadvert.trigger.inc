<?php
class PXTriggerDatabaseAbstractAdvert extends PXAbstractDatabaseTrigger {
	var $dtPlace;
	var $dtBanner;
	var $refName;

	var $isDynamic;
	var $oldPlaces;
	
	public function __get($name){
		switch($name){
			case 'db':
				return PXRegistry::getDb();
		}
	}

	function PXTriggerDatabaseAbstractAdvert() {
		$this->path       = BASEPATH.'site/var/ad/in';
		$app              = PXRegistry::getApp();
		$settings         = $app->modules['advert']->settings;
		
		$this->dtBanner   = $app->types[$settings['banner']];
		$this->dtPlace    = $app->types[$settings['place']];
		$this->refName    = $app->references[$settings['banner'].'2'.$settings['place']];
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

	function OnModifyObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB) {
		if ($format->id == 'adplace' || $format->id == 'adbanner') {
			$this->adSync($format->id, $object);
		}
	}

	function OnModifyLinks(&$db, &$objId, &$formatId, &$arLinks, $linkedFormat) {
		if ($formatId == 'adplace' || $formatId == 'adbanner') {
			$object = $this->db->GetObjectById($this->db->app->types[$formatId], $objId);
			$this->adSync($formatId, $object);
		}
	}

	function OnBeforeModifyLinks(&$db, &$objId, &$formatId, &$links, $linkedFormat, &$objectInDB, $reference) {
		if ($formatId == 'adbanner') {
			$this->oldPlaces = $this->getPlacesById($objId);
		}
		return true;
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
		$this->_writeCache($name, implode("\n", array_map(create_function('$row', ' return str_replace("\n", " ", $row);'), $this->writeBanner($object))) . "\n", $object['status']);

		$places  = $this->getPlacesById($object['id']);

		if ($this->oldPlaces) {
			$places = $places + $this->oldPlaces;
		}

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

	/** reload file */
	protected $reloadFD = null;

	protected function lockReload() {
		if ($this->reloadFD) {
			trigger_error('Reload flag was locked already.');
			return;
		}
		$this->reloadFD = fopen($this->path."/reload", "r");
		if (!flock($this->reloadFD, LOCK_EX)) {
			fclose($this->reloadFD);
			$this->reloadFD = null;
			trigger_error("Couldn't get the lock of reload flag!");
		}
	}

	protected function unlockReload() {
		if (!$this->reloadFD) {
			trigger_error('Try to unlock nonlocked reload flag');
			return;
		}
		if (!flock($this->reloadFD, LOCK_UN) || !fclose($this->reloadFD)) {
			trigger_error('Error while unlocking reload flag');
		}
		$this->reloadFD = null;
	}

	protected function touchReload() {
		touch($this->path.'/reload');
		usleep(0.3e+9); // let it be 0.3 sec instead of sleep(1). but wtf?
	}

	function adSync($format, $object) {
		if($object != null) {
			if (isset($object['dynamic'])) {
				$this->isDynamic = $object['dynamic'] ? 1 : 0;
			}
			
			if (!isset($object['reference']) || !trim($object['reference'])) {
				$object['reference'] = '#';
			}

			$this->lockReload();

			$this->$format($object);

			$this->touchReload();

			$this->unlockReload();
		}
	}
}
?>
