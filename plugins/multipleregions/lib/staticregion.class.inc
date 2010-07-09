<?php
class PXCorpRegions {
	static function isFederalRegion($id = NULL) {
		if (is_null($id)) {
			$id = PXMultiRegions::getUserRid();
		}
		
		$r  = PXRegistry::getDb()->query('SELECT federal FROM sys_regions WHERE id = '. $id . 'LIMIT 1');

		return isset($r[0]) ? $r[0]['federal'] == 't' : 'f';
	}
	
	static function getRegionId($type = 'user') {
		$r = PXMultiRegions::getInstance();

		if ($type == 'user') {
			return  $r->getUserRid();
		} else {
			return $r->getAdminRid();
		}
	}
	
	static function getAllData() {
		return PXMultiRegions::getRegionsData();
	}
	
	static function getRegionData($id = NULL) {
		if ($id == NULL) {
			$r = PXMultiRegions::getInstance();
			return $r->getMyRegionData();
		} else {
			return PXRegistry::getDb()->getObjectById(PXRegistry::getApp()->types['sys_regions'], $id);
		}
	}
	
	static function getField() {
		return PXMultiRegions::REGION_MARK;
	}
	
	static function getAllRegionsObjects($tree, $objType, $where) {
		$tmp     = PXRegistry::getDb()->Query('SELECT * FROM ' . $objType . ' WHERE '. $where);
		$rData   = self::getAllData();
		$objects = array();

		foreach ($tmp as $obj) {
			if (isset($obj[self::getField()])) {
				$obj[self::getField()] = str_replace('{', '', $obj[self::getField()]);
				$obj[self::getField()] = str_replace('}', '', $obj[self::getField()]);
				$obj[self::getField()] = explode(',', $obj[self::getField()]);
	
				foreach ($rData as $val) {
					if(in_array($val['id'], $obj[self::getField()])) {
						$obj['fullpath'] = 'http://' . trim(current(explode(',', $val['url']))) . createPathByParentId($tree, $obj['parent']) . $obj['pathname'];
						break;
					}
				}
			}
			
			$objects[$obj['id']] = $obj;
		}
				
		return $objects;
	}
	
	static function whereRegion($type){
		$rid = self::getRegionId($type);
		
		if($rid != 0){
			$where = PXRegistry::getDb()->inArray('sys_regions', $rid);
		} else {
			$where = PXRegistry::getDb()->TrueStatusString();
		}
		return $where;
	}
	
	static function isRidInTree($sid, $rid) {
		$data = PXRegistry::getDb()->Query('SELECT * FROM struct WHERE id = '.$sid.' AND sys_regions @> ARRAY['.$rid.']');
		
		return count($data);
	}
	
	static function isObjectInRegion($objType, $pathName, $rid) {
		$db = PXRegistry::getDb();
		$obj = array();

		if (isset($db->types[$objType]) && isset($db->types[$objType]->fields['pathname'])) {
			$regionWhere = '';
			
			if (isset($db->types[$objType]->fields['sys_regions'])) {
				$regionWhere = " AND sys_regions @> ARRAY[".$rid."]";
			}

			$obj = $db->query("SELECT * FROM ".$objType." WHERE pathname = '".$pathName."'".$regionWhere);
		}
		
		return $obj;
	}
	
	static function getAllUrls() {
		$urls    = array();
		$regions = PXRegistry::getDb()->getObjects(PXRegistry::getDb()->types['sys_regions'], true);
		
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
			$engPrfx = PXRegistry::getApp()->getProperty('ENGLISH_HOST_PREFIX', 'english');
			
			if (strstr($host, $engPrfx)) {
				$host = str_replace($engPrfx, '', $host);
				$engVersion = $engPrfx;
			}

			$intersection = array(
				'count' => 0,
				'url'   => 0
			);
			
			foreach ($urls as $k => $url) {
				$posibleHost = explode('.', $url);
				$currentHost = explode('.', $host);

				$count = count(array_intersect($posibleHost, $currentHost));
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
	
	static function switchRegion($tree, $fullPath, $regionId, $sidFrom) {
		if (!self::regionExists($regionId)) {
			return $fullPath;
		}
		
		$fullPath = str_replace('http://', '', $fullPath);
		$path     = explode('/', $fullPath);
		$file     = '';

		if (strpos(end($path), '.html')) {
			$file = end($path);
		}

		if (!count(PXRegistry::getDb()->getObjectById(PXRegistry::getDb()->types['struct'], $sidFrom))) {
			$sidFrom = PXRegistry::getDb()->getObjectsByWhere(PXRegistry::getDb()->types['struct'], true, "type = 'root'");
			$sidFrom = key($sidFrom);
		}

		$findSid = true;
		$sid     = $sidFrom;

		do {
			if (self::isRidInTree($sid, $regionId)) {
				$findSid = false;
			} else {
				$s      = PXRegistry::getDb()->getObjectById(PXRegistry::getDb()->types['struct'], $sid);
				$parent = PXRegistry::getDb()->getObjectById(PXRegistry::getDb()->types['struct'], $s['parent']);
				$sid    = $parent['id'];
			}
		} while ($findSid);
		
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
		
		$localPath = createPathByParentId($tree, $sid);
		$host = self::findUrl($path[0], $regionId);

		return 'http://'.$host.$localPath.$file;
	}
	
	static function regionExists($id) {
		return array_key_exists($id, self::getAllData());
	}
}
?>