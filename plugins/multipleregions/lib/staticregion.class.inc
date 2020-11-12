<?php

/**
 * @deprecated DONT USE IT NEVER IN NEW SUBPROJECTS. DEFINITELY UNEXPECTABLE CODE
 * @todo split to interfaces (IRegionsHelper, ...), and to abstract classes with static methods (or just simple functions)
 */
class PXCorpRegions
{
	private static $fullTree;

	public static function multi()
	{
		return PXMultiRegions::getInstance();
	}

	public static function isFederalRegion($id = NULL)
	{
		if (is_null($id)) {
			$id = self::multi()->getUserRid();
		}

		$r = PXRegistry::getDb()->query(sprintf('SELECT federal FROM %s WHERE id = %d LIMIT 1', self::getField(), (int)$id));

		return isset($r[0]) && $r[0]['federal'] == 't';
	}

	public static function getFederalHost($host)
	{
		$fId = PXRegistry::getDb()->query(sprintf('SELECT id FROM %s WHERE federal LIMIT 1', self::getField()));

		return self::findUrl($host, $fId[0]['id']);
	}

	public static function getRegionId($type = 'user')
	{
		$r = self::multi();

		if ($type == 'user') {
			return $r->getUserRid();
		} else {
			return $r->getAdminRid();
		}
	}

	public static function getRidByHost($hostname)
	{
		return self::multi()->getUserRid($hostname);
	}

	public static function getAllData()
	{
		return self::multi()->getRegionsData();
	}

	public static function getRegionData($id = NULL)
	{
		$r = self::multi();
		if ($id == NULL) {
			return $r->getMyRegionData();
		} else {
			return getFromArray($r->getRegionsData(), $id, []);
		}
	}

	public static function getField()
	{
		return PXMultiRegions::REGION_MARK;
	}

	public static function getAllRegionsObjects($tree, $objType, $where)
	{
		$tmp = PXRegistry::getDb()->Query('SELECT * FROM ' . $objType . ' WHERE ' . $where); //FIXME ! It is safe query ???
		$host = PXRegistry::getRequest()->getHttpHost();
		$rData = self::getAllData();
		$objects = [];

		foreach ($tmp as $obj) {
			if (isset($obj[self::getField()])) {
				$obj[self::getField()] = str_replace('{', '', $obj[self::getField()]);
				$obj[self::getField()] = str_replace('}', '', $obj[self::getField()]);
				$obj[self::getField()] = explode(',', $obj[self::getField()]);

				foreach ($rData as $val) {
					if (in_array($val['id'], $obj[self::getField()])) {
						$obj['fullpath'] = 'http://' . self::findUrl($host, $val['id']) . createPathByParentId($tree, $obj['parent']) . $obj['pathname'];
						break;
					}
				}
			}

			$objects[$obj['id']] = $obj;
		}

		return $objects;
	}

	public static function whereRegion($type)
	{ //FIXME ! Possible wrong behaviour ! See PXMultiRegions::findByRegion()
		$rid = self::getRegionId($type);

		if ($rid != 0) {
			$where = PXRegistry::getDb()->inArray(self::getField(), $rid);
		} else {
			$where = PXRegistry::getDb()->TrueStatusString();
		}
		return $where;
	}

	public static function isRidInTree($sid, $rid)
	{
		if (!self::$fullTree) {
			self::initFullTree();
		}

		if (isset(self::$fullTree->leafs[$sid]) && !self::$fullTree->leafs[$sid]->isRoot()) {
			$regions = explode(',', preg_replace('/({|})/', '', self::$fullTree->leafs[$sid]->content[self::getField()]));
			return in_array($rid, $regions);
		}

		return false;
	}

	public static function isObjectInRegion($objType, $pathName, $rid)
	{
		$db = PXRegistry::getDb();
		$obj = [];

		if (isset($db->types[$objType]) && isset($db->types[$objType]->fields['pathname'])) {
			$regionWhere = [sprintf("pathname = '%s'", $pathName)]; //FIXME ! It is safe query ?

			if (isset($db->types[$objType]->fields[self::getField()])) {
				$regionWhere[] = $db->inArray(self::getField(), $rid);
			}

			$obj = $db->query(sprintf("SELECT * FROM %s WHERE %s", $objType, join(' AND ', $regionWhere)));
		}

		return $obj;
	}

	public static function getAllUrls()
	{
		$urls = [];
		$regions = PXRegistry::getDb()->getObjects(PXRegistry::getDb()->types[self::getField()], true);

		if (!count($regions)) {
			return $urls;
		}

		foreach ($regions as $region) {
			if (!empty($region['url'])) {
				$urls = array_merge($urls, explode(',', trim($region['url'])));
			}
		}

		return $urls;
	}

	public static function findUrl($host, $regionId)
	{
		$host = mb_strtolower($host);

		$regionData = self::getRegionData($regionId);
		$urls = explode(',', $regionData['url']);

		$langVersion = '';
		$newHost = '';
		//TODO: for multilanguages there must be an ENUM
		$langPrefix = PXRegistry::getApp()->getProperty('ENGLISH_HOST_PREFIX', 'english');

		if (count($urls)) {
			if (self::isLangHost($host)) {
				$host = str_replace($langPrefix . '.', '', $host);
				$langVersion = $langPrefix . '.';
			}

			$intersection = [
				'count' => 0,
				'url' => null,
			];

			$currentHost = array_reverse(explode('.', $host));

			foreach ($urls as $k => $url) {

				//FIXME! WTF IS HERE ??? skipping special hosts. buggy and unexpectable
				if (preg_match(sprintf('/^(%s\.|pda\.|m\.)/', $langPrefix), $url)) {
					continue;
				}

				$possibleHost = array_reverse(explode('.', $url));
				$count = 0;

				// filling up new host name
				foreach ($possibleHost as $k => $v) {
					if (!isset($currentHost[$k]) || ($v !== '*' && $v !== $currentHost[$k])) {
						break;
					}
					$count++;
					$possibleHost[$k] = $currentHost[$k];
				}

				$possibleHost = join('.', array_reverse($possibleHost));

				if ($count > $intersection['count'] && !mb_strstr($possibleHost, '*')) {
					$intersection['count'] = $count;
					$intersection['url'] = $possibleHost;
				}
			}

			$newHost = $intersection['url'];
		}

		// cuz that $langVersion here we can't instantly prepend any other domain prefixes (pda., m., etc.)
		$newHost = $langVersion . (mb_strlen($newHost) ? $newHost : $host);

		if (count(explode('.', $newHost)) == 2) { // special behaviour for cases like 'domain.name'
			$newHost = 'www.' . $newHost;
		}

		return $newHost;
	}

	public static function switchRegion($tree, $fullPath, $regionId, $sidFrom = null)
	{
		if (!self::regionExists($regionId)) {
			return $fullPath;
		}

		$urlData = parse_url($fullPath) + ['host' => '', 'path' => ''];
		$path = explode('/', ltrim($urlData['path'], '/'));

		$pathname = end($path);
		array_unshift($path, self::isLangHost($urlData['host']) ? 'en' : 'default'); //TODO: change this root finding method !

		if (empty($tree)) {
			$tree = self::initRegionTree($regionId, $path);
		}

		// cuz we need to restore current struct (path) in new region instantly. TODO: think about how to move it outta here
		$path = $tree->getIdArrayByPath(OBJ_FIELD_PATHNAME, $path);

		while (($sid = array_pop($path)) < 0) ;

		if (mb_strpos($pathname, '.') && isset($tree->leafs[$sid])) {
			$db = PXRegistry::getDb();
			$query = [];
			$allowedFormats = PXRegistry::getApp()->getAllowedChilds(DT_STRUCT, $tree->leafs[$sid]);

			foreach ($allowedFormats as $format => $v) {
				if (!isset(PXRegistry::getTypes($format)->fields[OBJ_FIELD_PATHNAME])) {
					continue;
				}
				$query[] = sprintf("SELECT id FROM %s WHERE parent = %d AND %s='%s'", $format, $sid, OBJ_FIELD_PATHNAME, $db->escapeString($pathname));
			}

			sizeof($query) && $result = $db->query(join(' UNION ', $query));

			if (empty($result)) {
				$pathname = '';
			}
		} else {
			$pathname = '';
		}

		return http_build_url($fullPath, [
			'host' => self::findUrl($urlData['host'], $regionId),
			'path' => '/' . createSomePathByParentId($tree, $sid, OBJ_FIELD_PATHNAME, '/', true, false) . $pathname,
		], HTTP_URL_REPLACE);
	}

	public static function regionExists($id)
	{
		return array_key_exists($id, self::getAllData());
	}

	public static function initFullTree()
	{
		if (empty(self::$fullTree)) {
			$db = PXRegistry::getDb();
			$treeTable = $db->Query('SELECT * FROM struct');
			self::$fullTree = $db->buildTree($treeTable);
		}

		return self::$fullTree;
	}

	public static function initRegionTree($id, $pathnames = null)
	{
		$db = PXRegistry::getDb();
		$region = PXMultiRegions::getInstance();
		$current_id = $region->getRid();
		$region->setRid($id, true);

		if (!empty($pathnames)) {
			$where = [];
			foreach ($pathnames as $pathname) {
				mb_strlen($pathname) && $where[] = sprintf("'%s'", $db->escapeString($pathname));
			}
			$where = sprintf('pathname IN (%s)', join(',', array_unique($where)));
			$tree = $db->getObjectsByWhere(PXRegistry::getTypes(DT_STRUCT), true, $where, DB_SELECT_TREE);
		} else {
			$tree = $db->getObjects(PXRegistry::getTypes(DT_STRUCT), true, DB_SELECT_TREE);
		}

		$region->setRid($current_id, true);
		return $tree;
	}

	public static function isLangHost($host)
	{
		//TODO: for multiple languages there must be EMUN with site languages
		$langPrefix = PXRegistry::getApp()->getProperty('ENGLISH_HOST_PREFIX', 'english');
		return mb_strstr($host, $langPrefix . '.') ? $langPrefix : '';
	}

	public static function isRegionalObject($format_id)
	{
		return self::multi()->isRegionalObject(PXRegistry::getTypes($format_id));
	}
}
