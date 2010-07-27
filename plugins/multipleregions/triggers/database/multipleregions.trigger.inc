<?

class PXTriggerDatabaseMultipleregions extends PXAbstractDatabaseTrigger {
	function OnBeforeAddObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues) {
		$regions = $this->getRegions();

		$permit = $regions->checkRegionAccess($format, $object) && $regions->checkRegionAccessToParent($format, $object);

		if($permit) {
			$reflex_field = PXPublicRegionObjectCloner::REFLEX_FIELD;

			if(isset($object[$reflex_field])) {
				$this->simpleSetDbValues($reflex_field, $object[$reflex_field], $dbFields, $dbValues);
			}

			$regions->addRegionByDefault($format, $dbFields, $dbValues);
		} else {
			$this->fireError();
		}

		return $permit;
	}

	function OnBeforeBuildTree($db, $format, &$table, $parentName) {/*{{{*/
		$regions = $this->getRegions();

		if(!$regions->isFederal()) {
			$refl  = @array_flip(GetColFromTableWithIndexs($table, PXPublicRegionObjectCloner::REFLEX_FIELD));

			foreach($table as $id => $t)  {
				if(isset($refl[$t['parent']])) {
					$table[$id]['parent'] = $refl[$t['parent']];
				}
			}
		}
	}/*}}}*/

	function OnBeforeModifyObject(&$db, $objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB) {/*{{{*/
		$regions = $this->getRegions();

		$permit = $regions->checkRegionAccess($format, $objectInDB) && $regions->checkRegionAccessToParent($format, $objectInDB);

		if(!$permit && $regions->isRegionalObject($format)) {
			$regionsDirectory = $regions->loadDirectory();

			$cloner = 
				new PXPublicRegionObjectCloner($format, $object, $objectInDB, $dbFields, $dbValues, $this);

			$nid = $cloner->cloneObject($regions->loadDirectory()->values);

			$request = PXRegistry::getRequest();
			PXResponse::getInstance()->redirect("popup.phtml?area={$request->getArea()}&format={$format->id}&id={$nid}&action=main");
		}

		if($permit) {
			$regions->addRegionByDefault($format, $dbFields, $dbValues);
		} else {
			$this->fireError();
		}

		return $permit;
	}/*}}}*/

	function OnModifyObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB) {/*{{{*/
	}/*}}}*/

	function OnBeforeDeleteObject(&$db, &$objId, &$format, &$objectInDB) {/*{{{*/
		$regions = $this->getRegions();

		$permit = $regions->checkRegionAccess($format, $objectInDB) && $regions->checkRegionAccessToParent($format, $objectInDB);

		if(!$permit && $regions->isRegionalObject($format)) {
			$cloner = 
				new PXPublicRegionObjectCloner($format, $object = null, $objectInDB, $dbFields = null, $dbValues = null, $this);

			$cloner->deleteObject(array_keys($regions->loadDirectory()->values));
		} elseif(!$permit) {
			$this->fireError();
		}

		return $permit;
	}/*}}}*/

	function OnDeleteObject(&$db, &$objId, &$format, &$objectInDB) {
	}

	function OnBeforeModifyLinks(&$db, &$objId, &$formatId, &$links) {
		return true;
	}

	function OnModifyLinks(&$db, &$objId, &$formatId, &$links) { // simple
	}

	function OnBeforeMoveObject(&$db, $objId, &$format, &$objectInDB) {
		$regions = $this->getRegions();

		#fixme
		$permit = $regions->checkRegionAccess($format, $objectInDB) && $regions->checkRegionAccessToParent($format, $objectInDB);

		if(!$permit && $regions->isRegionalObject($format)) {
			$cloner = 
				new PXPublicRegionObjectCloner($format, $objId, $objectInDB, $dbFields = null, $dbValues = null, $this);

			$cloner->moveObject(array_keys($regions->loadDirectory()->values));
		} elseif(!$permit) {
			$this->fireError();
		}

		return $permit;
	}

	function OnBeforeGetDataBuildQuery($db, $format, &$mode, &$selector, &$where, &$order, &$limit){
		$regions = $this->getRegions();
		$where = $this->reflexer($db, $regions, $where, $format);

		if($regions->isRegionalObject($format)) {
			$regionFilter = $regions->findByRegion();
			$where = strlen($where) ? "($where) AND $regionFilter" : $regionFilter;

			if($mode != DB_SELECT_COUNT) {
				$reflex_field = PXPublicRegionObjectCloner::REFLEX_FIELD;
				$selector .= ", {$reflex_field}";
			}
		}
	}

	#fixme user registry
	function reflexer($db, $regions, $where, $format) {
		$userRegistry = PXRegistry::getHKCU();

		if(!$userRegistry->get('wheres')) {
			$userRegistry->set("wheres", array());
		}

		if(!strlen($where)) { 
			return $where;
		}

		$wheres         = &$userRegistry->getByRef("wheres");
		$oldWhere       = $where = preg_replace("#\s+#", " ", $where);
		$dest           = $userRegistry->get('dest');
		$types          = PXRegistry::getApp()->types;

		$isRegObj       = $regions->isRegionalObject($format);
		$isParentRegObj = isset($types[$format->parent]) ? $regions->isRegionalObject($types[$format->parent]) : false;

		if(isset($wheres[$where])) {
			return $wheres[$where];
		}

		$run = 1;

		#2 current format is regional or him parent regional? 
		switch(true) {
			case $isRegObj:
				$type = $format;
				$parent = null; 
				break;
			case $isParentRegObj:
				$type = $format;
				$parent = PXRegistry::getApp()->types[$format->parent];
				break;
			default:
				$run = 0;
		}

		#3 if 2->true try find reflex id
		if($run) {
			$dest = $this->getReflexId($type, $parent, $wheres, $where, $dest);
		}

		#4 if 3->finded change sql
		if(strlen($dest)) {
			#5
			#in operator
			$where = preg_replace("#({$format->id}\.)?(parent) in ([^\)\s]+)#i", "$1$2 in $3,'{$dest}'", $where);

			#= operator
			$where = preg_replace("#({$format->id}\.)?(parent) = ([^\)\s]+)#i", "$1$2 in ($3,'{$dest}')", $where);
		}

		#5 memomize where
		$wheres[$oldWhere] = $where;

		return $where;
	}

	function getReflexId($type, $parent, $wheres, $where, $dest) {
		$userRegistry = PXRegistry::getHKCU();
		$dest = $userRegistry->get("dest");

		preg_match(sprintf("#%s\.(id|parent) = [^0-9]*?([0-9]+)#", $type->id) , $where, $m);

		if(!isset($m[2]) || (strlen($dest) && $dest == $m[2]) || $userRegistry->get('type') == $type->parent) {
			return $dest;
		}

		$reflex_field = PXPublicRegionObjectCloner::REFLEX_FIELD;

		$result = pos((array) PXRegistry::getDB()->query($s = sprintf("select id, {$reflex_field} as dest from %s where %s = '%s'", 
			$parent ? $parent->id : $type->id, $parent ? "id" : $m[1], $m[2])));

		if(empty($result)) {
			return $dest;
		}

		if(!is_null($result['dest']))  {
			$userRegistry->set('dest', $result['dest']);
			$userRegistry->set('type', $type->parent);
		}

		return $userRegistry->get('dest');
	}

	function OnBeforeNormalizePathBuildQuery($objectFormat, $exObjectFormat, &$table, $object) {
		$regions = $this->getRegions();

		if($regions->isRegionalObject($objectFormat) && $regions->isRegionalObject($exObjectFormat)) {
			$regionFilter = $regions->findByRegion($object[PXMultiRegions::REGION_MARK]);

			if(strlen($table['exWhere'])) {
				$table['exWhere'] .=  " AND ";
			}

			$table['exWhere'] .= $regionFilter;
		}
	}

	function getRegions(){
		return PXMultiRegions::getInstance();
	}

	function fireError($message='Доступ к этому объекту/действию запрещен !'){
		$request  = PXRegistry::getRequest();
		$returnTo = $request->GetHttpReferer();
		
		if(empty($returnTo)){
			$returnTo = '/admin';
		}

		echo <<<JS
			<script type="text/javascript">
				alert('{$message}');
				window.location.href = '{$returnTo}';
			</script>
JS;
		exit;
	}
	
	function simpleSetDbValues($key ,$value, &$dbFields, &$dbValues) {
		$keys = array_flip($dbFields);
		
		if(!isset($keys[$key])) {
			$dbFields[] = $key;
			$dbValues[] = $value;
		} else {
			$dbValues[$keys[$key]] = $value;
		}
	}
}
?>
