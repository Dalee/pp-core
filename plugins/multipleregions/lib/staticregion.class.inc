<?php
class PXCorpRegions {
	private static $fullTree;

	static function isFederalRegion($id = NULL) {
		if (is_null($id)) {
			$id = PXMultiRegions::getUserRid();
		}
		
		$r = PXRegistry::getDb()->query(sprintf('SELECT federal FROM %s WHERE id = %d LIMIT 1', self::getField(), (int)$id));

		return isset($r[0]) ? $r[0]['federal'] == 't' : 'f';
	}

	static function getFederalHost($host) {
		$fId = PXRegistry::getDb()->query(sprintf('SELECT id FROM %s WHERE federal LIMIT 1', self::getField()));

		return self::findUrl($host, $fId[0]['id']);
	}

	static function getRegionId($type = 'user') {
		$r = PXMultiRegions::getInstance();

		if ($type == 'user') {
			return  $r->getUserRid();
		} else {
			return $r->getAdminRid();
		}
	}

	static function getRidByHost($hostname){
		return PXMultiRegions::getInstance()->getUserRid($hostname);
	}

	static function getAllData() {
		return PXMultiRegions::getRegionsData();
	}

	static function getRegionData($id = NULL) {
		if ($id == NULL) {
			$r = PXMultiRegions::getInstance();
			return $r->getMyRegionData();
		} else {
			return PXRegistry::getDb()->getObjectById(PXRegistry::getApp()->types[self::getField()], $id);
		}
	}

	static function getField() {
		return PXMultiRegions::REGION_MARK;
	}

	static function getAllRegionsObjects($tree, $objType, $where) {
		$tmp     = PXRegistry::getDb()->Query('SELECT * FROM ' . $objType . ' WHERE '. $where); //FIXME ! It is safe query ???
		$host    = PXRegistry::getRequest()->getHttpHost();
		$rData   = self::getAllData();
		$objects = array();

		foreach ($tmp as $obj) {
			if (isset($obj[self::getField()])) {
				$obj[self::getField()] = str_replace('{', '', $obj[self::getField()]);
				$obj[self::getField()] = str_replace('}', '', $obj[self::getField()]);
				$obj[self::getField()] = explode(',', $obj[self::getField()]);
	
				foreach ($rData as $val) {
					if(in_array($val['id'], $obj[self::getField()])) {
						$obj['fullpath'] = 'http://' . self::findUrl($host, $val['id']) . createPathByParentId($tree, $obj['parent']) . $obj['pathname'];
						break;
					}
				}
			}
			
			$objects[$obj['id']] = $obj;
		}
				
		return $objects;
	}

	static function whereRegion($type){ //FIXME ! Possible wrong behaviour ! See PXMultiRegions::findByRegion()
		$rid = self::getRegionId($type);
		
		if($rid != 0){
			$where = PXRegistry::getDb()->inArray(self::getField(), $rid);
		} else {
			$where = PXRegistry::getDb()->TrueStatusString();
		}
		return $where;
	}

	static function isRidInTree($sid, $rid) {
		if (!self::$fullTree) {
			self::initFullTree();
		}

		if (isset(self::$fullTree->leafs[$sid]) && !self::$fullTree->leafs[$sid]->isRoot()) {
			$regions = explode(',', preg_replace('/({|})/', '', self::$fullTree->leafs[$sid]->content[self::getField()]));
			return in_array($rid, $regions);
		}

		return false;
	}

	static function isObjectInRegion($objType, $pathName, $rid) {
		$db = PXRegistry::getDb();
		$obj = array();

		if (isset($db->types[$objType]) && isset($db->types[$objType]->fields['pathname'])) {
			$regionWhere = array(sprintf("pathname = '%s'", $pathName)); //FIXME ! It is safe query ?
			
			if (isset($db->types[$objType]->fields[self::getField()])) {
				$regionWhere[] = $db->inArray(self::getField(), $rid);
			}

			$obj = $db->query(sprintf("SELECT * FROM %s WHERE %s", $objType, join(' AND ', $regionWhere)));
		}
		
		return $obj;
	}

	static function getAllUrls() {
		$urls    = array();
		$regions = PXRegistry::getDb()->getObjects(PXRegistry::getDb()->types[self::getField()], true);
		
		if (!count($regions)) {
			return $urls;
		}

		foreach ($regions as $region) {
			if(!empty($region['url'])) {
				$urls = array_merge($urls, explode(',', trim($region['url'])));
			}
		}
		
		return $urls;
	}

	static function findUrl($host, $regionId) {
		$regionData = self::getRegionData($regionId);
		$urls       = explode(',', $regionData['url']);

		$engVersion = '';
		$newHost    = '';

		if (count($urls)) {
			$engPrfx = PXRegistry::getApp()->getProperty('ENGLISH_HOST_PREFIX', 'english').'.';
			
			if (strstr($host, $engPrfx)) {
				$host = str_replace($engPrfx, '', $host);
				$engVersion = $engPrfx;
			}

			$intersection = array(
				'count' => 0,
				'url'   => 0
			);

			foreach ($urls as $k => $url) {
				//FIXME
				if (preg_match('/^('.str_replace('.', '\.', $engPrfx).'|pda\.|m\.)/', $url)) {
					continue;
				}

				$possibleHost = explode('.', $url);
				$currentHost  = explode('.', $host);

				$count = count(array_intersect($possibleHost, $currentHost));
				if ($count > $intersection['count']) {
					$intersection['count'] = $count;
					$intersection['url']   = $urls[$k];
				}
			}

			$newHost = $intersection['url'];

			if (strstr($newHost, '*')) {
				$tmpNewHost     = array_reverse(explode('.', $newHost));
				$tmpCurrentHost = array_reverse($currentHost);

				foreach ($tmpNewHost as $k => $v) {
					if ($v == '*') {
						$tmpNewHost[$k] = $tmpCurrentHost[$k];
					}
				}

				$newHost = implode('.', array_reverse($tmpNewHost));
			}
		}

		$newHost = (strlen($newHost) ? $newHost : $host);
		return $engVersion . trim($newHost);
	}

	static function switchRegion($tree, $fullPath, $regionId, $sidFrom = null) {
		if (!self::regionExists($regionId)) {
			return $fullPath;
		}

		$fullPath = str_replace('http://', '', $fullPath);
		$path     = explode('/', $fullPath);

		$file     = '';

/*
		FIXME !!!

		if (strpos(end($path), '.html')) {
			$file = end($path);
		}

		$ancestors = $tree->getAncestors($sidFrom);

		array_unshift($ancestors, $sidFrom);

		if (!count($tree->getFullPath($sidFrom)) && isset($ancestors[count($ancestors)-2])) {
			$sidFrom = $ancestors[count($ancestors)-2];
		}

		$sid = $sidFrom;

		foreach ($ancestors as $a) {
			if (self::isRidInTree($a, $regionId)) {
				$sid = $a;
				break;
			}
		}

		if ($sid == $sidFrom && $file) {
			$allowed = $tree->leafs[$sid]->content['allowed'];
			$obj = array();

			foreach ($allowed as $k => $v) {
				$obj = self::isObjectInRegion($k, $file, $regionId);
			}

			if (!count($obj)) {
				$file = '';
			}
		}

		$localPath = '/'.createSomePathByParentId($tree, $sid, 'pathname', '/', true, false);
*/

		$host = self::findUrl($path[0], $regionId);

		#fixme!!! если страницы нет в регионе вернется 404
		array_shift($path);

		return sprintf('http://%s/%s', $host, join("/", $path));
	}

	static function regionExists($id) {
		return array_key_exists($id, self::getAllData());
	}

	static function initFullTree() {
		if(empty(self::$fullTree)){
			$treeTable      = PXRegistry::getDb()->Query('SELECT * FROM struct');
			self::$fullTree = new NLTree($treeTable);
		}
	}

	static function initRegionTree($id) {
		$db = PXRegistry::getDb();
		$treeTable = $db->query(sprintf("SELECT * FROM struct WHERE %s", $db->inArray(self::getField(), $id)));
		return new NLTree($treeTable);
	}
}
?>
