<?

class PXTriggerDatabaseMultipleregions extends PXAbstractDatabaseTrigger {
	function OnBeforeAddObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues) {
		$regions = $this->getRegions();

		if(isset($object['copy'])) {
			$this->simpleSetDbValues('sys_regions', array(), $dbFields, $dbValues);
			$this->simpleSetDbValues('reflex_id', $object['reflex_id'], $dbFields, $dbValues);
		}

		$regions->addRegionByDefault($format, $dbFields, $dbValues);

		$permit = $regions->checkRegionAccessToParent($format, $object);

		if(!$permit){
			$this->fireError();
		}
	
		return $permit;
	}

	function OnBeforeBuildTree($db, $format, &$table, $parentName) {
		$regions = $this->getRegions();

		if($regions->getRid()) {
			$refl  = @array_flip(GetColFromTableWithIndexs($table, 'reflex_id'));

			foreach($table as $id => $t)  {
				if(isset($refl[$t['parent']])) {
					$table[$id]['parent'] = $refl[$t['parent']];
				}
			}
		}
	}

	function OnBeforeModifyObject(&$db, $objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB) {
		$regions = $this->getRegions();
		$rid = $regions->getRid();
		
		$p1 = false;

		if(isset($objectInDB['sys_regions'])) {
			$p1  = $regions->isInRegion('-1', $objectInDB['sys_regions']) 
				|| ($regions->isCurrentInSet($objectInDB['sys_regions'])
					&& $objectInDB['sys_regions'] != array($rid));
		}

		if($rid && $p1) {
			if(isset($objectInDB['deny_region_edit']) && !empty($objectInDB['deny_region_edit'])) {
				$this->fireError('Только федеральное редактирование!');
			}

			$cloner = 
				new PXPublicRegionObjectCloner($format, $object, $objectInDB, $dbFields, $dbValues, $rid);
			$cloner->cloneObject();

			$object['redir'] = $cloner->nid;
			return true;
		}

		$permit = $regions->checkRegionAccess($format, $objectInDB) && $regions->checkRegionAccessToParent($format, $objectInDB);

		if($permit){
			$regions->addRegionByDefault($format, $dbFields, $dbValues);
		} else {
			$this->fireError();
		}

		return $permit;
	}

	function OnModifyObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB) {
		if(isset($object['redir'])) {
			PXResponse::getInstance()->redirect("popup.phtml?area=objects&format={$format->id}&id={$object['redir']}&action=main");
		}
	}

	function OnBeforeDeleteObject(&$db, &$objId, &$format, &$objectInDB) {
		$regions = $this->getRegions();
		$permit = $regions->checkRegionAccess($format, $objectInDB) && $regions->checkRegionAccessToParent($format, $objectInDB);

		if(!$permit){
			$this->fireError();
		}
		
		return $permit;
	}

	function OnDeleteObject(&$db, &$objId, &$format, &$objectInDB) {
	}

	function OnBeforeModifyLinks(&$db, &$objId, &$formatId, &$links) {
		return true;
	}

	function OnModifyLinks(&$db, &$objId, &$formatId, &$links) { // simple
	}

	function OnBeforeMoveObject(&$db, &$objId, &$format, &$objectInDB) {
		$regions = $this->getRegions();
		$permit  = $regions->checkRegionAccess($format, $objectInDB);
		if(!$permit){
			$this->fireError();
		}
		return $permit;
	}

	function OnBeforeGetDataBuildQuery($db, $format, &$mode, &$selector, &$where, &$order, &$limit){
		$regions = $this->getRegions();
		$where = $this->reflexer($db, $regions, $where, $format);

		if($regions->isRegionalObject($format)
			&& !PXRegistry::getHKCU()->get("parentdrop") 
			&& !PXRegistry::getHKCU()->get("mod")
			&& !PXRegistry::getHKCU()->get("add")) {
				$regionFilter = $regions->findByRegion();
				$where = strlen($where) ? "($where) AND $regionFilter" : $regionFilter;

				if($mode != DB_SELECT_COUNT) {
					$selector .= ", reflex_id";
				}

		}

		// d2($where);
	}

	function reflexer($db, $regions, $where, $format) {
		$userRegistry = PXRegistry::getHKCU();

		if(!$userRegistry->get('wheres')) {
			$userRegistry->set("wheres", array());
		}

		if(!strlen($where)) { 
			return $where;
		}

		$wheres         = &$userRegistry->getByRef("wheres");
		$rid            = $regions->getRid();
		$oldWhere       = $where = preg_replace("#\s+#", " ", $where);
		$dest           = $userRegistry->get('dest');
		$types 			= PXRegistry::getApp()->types;

		$isRegObj       = $regions->isRegionalObject($format);
		$isParentRegObj = isset($types[$format->parent]) ? $regions->isRegionalObject($types[$format->parent]) : false;

		if(isset($wheres[$where])) {
			return $wheres[$where];
		}

		#1 is region?
		if(!$rid) { 
			return $where;
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

		$result = pos((array) PXRegistry::getDB()->query($s = sprintf("select id, reflex_id as dest from %s where %s = '%s'", 
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
			$regionFilter = $regions->findByRegion();

			if(isset($object['copy'])) {
				$w = $table['exWhere'];
				
				$table['exWhere'] = "1 = -1" 
					. (strlen($w) ? " and " : "")
					. $w;
			}

			$table['exWhere'] .= strlen($table['exWhere']) ? " and " : "";
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
