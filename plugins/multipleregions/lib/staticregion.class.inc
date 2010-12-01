<?php
class PXCorpRegions {
	private static $fullTree;

	static function multi() {
		return PXMultiRegions::getInstance();
	}

	static function isFederalRegion($id = NULL) {
		if (is_null($id)) {
			$id = self::multi()->getUserRid();
		}
		
		$r = PXRegistry::getDb()->query(sprintf('SELECT federal FROM %s WHERE id = %d LIMIT 1', self::getField(), (int)$id));

		return isset($r[0]) && $r[0]['federal'] == 't';
	}

	static function getFederalHost($host) {
		$fId = PXRegistry::getDb()->query(sprintf('SELECT id FROM %s WHERE federal LIMIT 1', self::getField()));

		return self::findUrl($host, $fId[0]['id']);
	}

	static function getRegionId($type = 'user') {
		$r = self::multi();

		if ($type == 'user') {
			return  $r->getUserRid();
		} else {
			return $r->getAdminRid();
		}
	}

	static function getRidByHost($hostname){
		return self::multi()->getUserRid($hostname);
	}

	static function getAllData() {
		return self::multi()->getRegionsData();
	}

	static function getRegionData($id = NULL) {
		if ($id == NULL) {
			$r = self::multi();
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
		$host = strtolower($host);

		$regionData = self::getRegionData($regionId);
		$urls       = explode(',', $regionData['url']);

		$langVersion = '';
		$newHost     = '';
		//TODO: for multilanguages there must be an ENUM
		$langPrefix  = PXRegistry::getApp()->getProperty('ENGLISH_HOST_PREFIX', 'english');

		if (count($urls)) {
			if (self::isLangHost($host)) {
				$host        = str_replace($langPrefix . '.', '', $host);
				$langVersion = $langPrefix . '.';
			}
			
			$intersection = array(
				'count' => 0,
				'url'   => null
			);
			
			$currentHost  = array_reverse(explode('.', $host));
			
			foreach ($urls as $k => $url) {
				//FIXME! WTF IS HERE ???
				if (preg_match(sprintf('/^(%s\.|pda\.|m\.)/', $langPrefix), $url)) {
					continue;
				}

				$possibleHost = array_reverse(explode('.', $url));
				$count        = 0;
				
				foreach($possibleHost as $k => &$v) {
					if(isset($currentHost[$k]) && ($currentHost[$k] === $v || $v === '*')){
						$count++;
						$v = $currentHost[$k];
					} else {
						break;
					}
				}
				
				$possibleHost = join('.', array_reverse($possibleHost));
				
				if ($count > $intersection['count'] && !strstr($possibleHost, '*')) {
					$intersection['count'] = $count;
					$intersection['url']   = $possibleHost;
				}
			}

			$newHost = $intersection['url'];
		}

		$newHost = $langVersion . (strlen($newHost) ? $newHost : $host);
		
		if (count(explode('.', $newHost)) == 2) { //special behaviour for cases like 'megafon.ru'
			$newHost = 'www.' . $newHost;
		}
		
		return $newHost;
	}

	static function switchRegion($tree, $fullPath, $regionId, $sidFrom = null) {
		if (!self::regionExists($regionId)) {
			return $fullPath;
		}

		$fullPath = str_replace('http://', '', $fullPath);
		$path     = explode('/', $fullPath);
		
		if(empty($tree)) {
			$tree = self::initRegionTree($regionId);
		}
		
		$host = self::findUrl($path[0], $regionId);
		
		array_shift($path); //remove URL
		array_unshift($path, self::isLangHost($host) ? 'en' : 'default'); //TODO: change this root finding method !
		
		$path = $tree->getIdArrayByPath(OBJ_FIELD_PATHNAME, $path);
		while(($sid = array_pop($path)) < 0);
		$path = createSomePathByParentId($tree, $sid, OBJ_FIELD_PATHNAME, '/', true, false);

		return sprintf('http://%s/%s', $host, $path);
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

	static function isLangHost($host) {
		//TODO: for multiple languages there must be EMUN with site languages
		$langPrefix = PXRegistry::getApp()->getProperty('ENGLISH_HOST_PREFIX', 'english');
		return strstr($host, $langPrefix . '.') ? $langPrefix : '';
	}
	
	static function isRegionalObject($format_id){
		return self::multi()->isRegionalObject(PXRegistry::getTypes($format_id));
	}
}
?>
