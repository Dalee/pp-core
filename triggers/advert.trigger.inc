<?

class PXTriggerAdvert extends PXAbstractTrigger {

	var $dtPlace;
	var $dtCampaign;
	var $dtBanner;
	var $refName;

	function PXTriggerAdvert(&$settings) {
		$this->dtPlace    = $settings['place'];
		$this->dtCampaign = $settings['campaign'];
		$this->dtBanner   = $settings['banner'];
		$this->refName    = $this->dtBanner.'2'.$this->dtPlace;
		$this->path       = BASEPATH.'/var/ad/in';
	}

	function OnAddObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues) {
		if ($format->id == 'adplace' || $format->id == 'adbanner') {
			$this->adSync($db);
		}
	}

	function OnDeleteObject(&$db, &$objId, &$format) {
		if ($format->id == 'adplace' || $format->id == 'adbanner') {
			$this->adSync($db);
		}
	}

	function OnModifyObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues) {
		if ($format->id == 'adplace' || $format->id == 'adbanner') {
			$this->adSync($db);
		}
	}

	function OnModifyLinks(&$db, &$objId, &$formatId) {
		if ($formatId == 'adplace' || $formatId == 'adbanner') {
			$this->adSync($db);
		}
	}

	function writeBanner($object) {
		if (empty($object['image']['path']) && empty($object['flash']['path'])) {
			return;
		}
		$format = (empty($object['flash']['path'])) ? $object['type']."_gif" : $object['type']."_flash";
		$isDynamic = ($object['dynamic']) ? 1 : 0;
		$fd = fopen($this->path."/adv_b".$object['id'].".dat", "w");
		// flock($fd, LOCK_EX);
		fwrite($fd, $object['id']."\n");
		fwrite($fd, $format."\n");
		fwrite($fd, $isDynamic."\n");
		fwrite($fd, (int)$object['weight']."\n");
		fwrite($fd, $object['reference']."\n");
		fwrite($fd, $object['image']['path']."\n");
		fwrite($fd, "\n");
		fwrite($fd, $object['flash']['path']."\n");
		fwrite($fd, "white\n");
		// flock($fd, LOCK_UN);
		fclose($fd);
	}

	function writePlace($object, $attached, $banners) {
		$fd = fopen($this->path."/adv_p".$object['id'].".dat", "w");
		// flock($fd, LOCK_EX);
		fwrite($fd, $object['id']."\n");
		fwrite($fd, $object['type']."\n");
		foreach ($attached as $k=>$v) {
			if (isset($banners[$k]) && $banners[$k]['status']) {
				fwrite($fd, $k."\n");
			}
		}
		// flock($fd, LOCK_UN);
		fclose($fd);
	}

	function adSync(&$db) {
		$handle = opendir($this->path);
		if (!$handle) return;
		while (false !== ($file = readdir($handle))) {
			if (!preg_match("/^(adv_|)(b|p)\d+\.dat$/", $file)) continue;
			@unlink($this->path.'/'.$file);
		}
		closedir($handle);
		$places  = $db->GetObjects($db->app->types[$this->dtPlace], NULL);
		$banners = $db->GetObjects($db->app->types[$this->dtBanner], NULL);
		$fd = fopen($this->path."/reload", "r");
		flock($fd, LOCK_EX);
		foreach ($banners as $banner) {
			if ($banner['status']) {
				$this->writeBanner($banner);
			}
		}
		foreach ($places as $place) {
			if ($place['status']) {
				$attached = $db->GetLinks($db->app->references[$this->refName], $this->dtPlace, $place['id']);
				$this->writePlace($place, $attached, $banners);
			}
		}
		flock($fd, LOCK_UN);
		fclose($fd);

		touch($this->path.'/reload');
		sleep(1);
	}

}

?>
