<?php
class PXTriggerDatabaseMultipleregions extends PXAbstractDatabaseTrigger {
	function OnBeforeAddObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues) {
		$regions = $this->getRegions();

		$isRegionalObject  = $regions->isRegionalObject($format);
		$allowedRegionsIds = $regions->getAllowedRegionsIds();

		$rid = $regions->getRid();

		if($isRegionalObject && empty($allowedRegionsIds) && $rid) {
			$this->simpleSetDbValues(PXMultiRegions::REGION_MARK, "{{$rid}}", $dbFields, $dbValues);
			return true;
		}

		$permit = $regions->checkRegionAccess($format, $object) && $regions->checkRegionAccessToParent($format, $object);

		if($permit) {
			$reflex_field = PXMultipleRegionsReflexer::REFLEX_FIELD;

			if(isset($object[$reflex_field])) {
				$this->simpleSetDbValues($reflex_field, $object[$reflex_field], $dbFields, $dbValues);

				if(isset($object['allowed']) && is_array($object['allowed'])) {
					$this->simpleSetDbValues('allowed',  serialize($object['allowed']), $dbFields, $dbValues);
				}
			}

			$regions->addRegionByDefault($format, $dbFields, $dbValues);
		} else {
			$this->fireError();
		}

		return $permit;
	}

	function OnBeforeBuildTree($db, $format, &$table, $parentName) {
		$regions = $this->getRegions();

		if(!$regions->isFederal()) {
			$refl  = @array_flip(GetColFromTableWithIndexs($table, PXMultipleRegionsReflexer::REFLEX_FIELD));

			foreach($table as $id => $t)  {
				if(isset($t['parent']) && isset($refl[$t['parent']])) {
					$table[$id]['parent'] = $refl[$t['parent']];
				}
			}
		}
	}

	function OnBeforeModifyObject(&$db, $objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB) {
		$regions = $this->getRegions();

		$isRegionalObject  = $regions->isRegionalObject($format);
		$allowedRegionsIds = $regions->getAllowedRegionsIds();
		$regions_field = PXMultiRegions::REGION_MARK;

		if($isRegionalObject && empty($allowedRegionsIds)) {
			$this->simpleSetDbValues(PXMultiRegions::REGION_MARK, $objectInDB[$regions_field], $dbFields, $dbValues);
			return true;
		}

		$permit = $regions->checkRegionAccess($format, $objectInDB) && $regions->checkRegionAccessToParent($format, $objectInDB);

		if(!$permit && $isRegionalObject) {
			$cloner = 
				new PXPublicRegionObjectCloner($format, $object, $objectInDB, $dbFields, $dbValues, $this);

			$nid = $cloner->cloneObject($allowedRegionsIds);

			$request = PXRegistry::getRequest();

			if($this->isObjectForm()) {
				PXResponse::getInstance()->redirect("popup.phtml?area={$request->getArea()}&format={$format->id}&id={$nid}&action=main");
			} else {
				$hasBeenCloned = true; //if not redirected to popup after clone, do not fire error !
			}
		}

		if($permit) {
			if(isset($object['sys_order']) && is_numeric($object['sys_order']) && $object['sys_order'] > 0) {
				$this->simpleSetDbValues('sys_order', $object['sys_order'], $dbFields, $dbValues);
			}

			$regions->addRegionByDefault($format, $dbFields, $dbValues, $object);
		} elseif(empty($hasBeenCloned)){
			$this->fireError();
		}

		return $permit;
	}

	function OnBeforeDeleteObject(&$db, &$objId, &$format, &$objectInDB) {
		$regions = $this->getRegions();

		$permit = $regions->checkRegionAccess($format, $objectInDB) && $regions->checkRegionAccessToParent($format, $objectInDB);

		if(!$permit && $regions->isRegionalObject($format)) {
			$cloner = 
				new PXPublicRegionObjectCloner($format, $object = null, $objectInDB, $dbFields = null, $dbValues = null, $this);

			$cloner->deleteObject($regions->getAllowedRegionsIds()); //FIXME, i am broken in multioperations !
		} elseif(!$permit) {
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

	function OnBeforeMoveObject(&$db, $objId, &$format, &$objectInDB) {
		$regions = $this->getRegions();

		#fixme
		$permit = $regions->checkRegionAccess($format, $objectInDB) && $regions->checkRegionAccessToParent($format, $objectInDB);

		if(!$permit && $regions->isRegionalObject($format)) {
			$cloner = 
				new PXPublicRegionObjectCloner($format, $objId, $objectInDB, $dbFields = null, $dbValues = null, $this);

			$cloner->moveObject($regions->getAllowedRegionsIds());
		} elseif(!$permit) {
			$this->fireError();
		}

		return $permit;
	}

	#todo cache sql-queries
	function onBeforeCheckForParent($db, $format, &$object, &$parentObject) {

		$regions = $this->getRegions();
		$types   = PXRegistry::getApp()->types;

		if (!isset($types[$format->parent])) {
			return null;
		}

		if ($regions->isRegionalObject($types[$format->parent])) {
			# find original parent
			$parentObject = $db->GetObjectById($types[$format->parent], $object['parent']);

			if (!empty($parentObject)) {
				return true;
			}

			# find reflected parent
			$parentObject = $db->getObjectsByFieldLimited(
				PXRegistry::getApp()->types[$format->parent], null,
				PXMultipleRegionsReflexer::REFLEX_FIELD, $object['parent'], 1, 0);

			if(!empty($parentObject)) {
				$parentObject = reset($parentObject);
				return true;
			}

			# not founded parent. Fatal Error
			return false;
		}

		#standart answer for skip.
		return null;
	}

	function OnBeforeGetDataBuildQuery($db, $format, &$mode, &$selector, &$where, &$order, &$limit){

		$regions = $this->getRegions();

		if(strlen($where)) {
			$reflexer = new PXMultipleRegionsReflexer($regions, $where, $format);
			$where    = $reflexer->getWhere();
		}

		if($regions->isRegionalObject($format)) {
			$regionFilter = $regions->findByRegion();
			$where = strlen($where) ? "($where) AND $regionFilter" : $regionFilter;

			if($mode != DB_SELECT_COUNT) {
				$reflex_field = PXMultipleRegionsReflexer::REFLEX_FIELD;
				$selector .= ", {$format->id}.{$reflex_field}";
			}
		}
	}

	function isObjectForm() {
		return PXRegistry::getApp()->isAdminEngine() && PXRegistry::getRequest()->issetVar(PXMultiRegions::REGION_MARK . "_objectform");
	}

	function OnBeforeNormalizePathBuildQuery($objectFormat, $exObjectFormat, &$table, $object) {
		$regions = $this->getRegions();

		$regionsField = PXMultiRegions::REGION_MARK;
		$isRegionalObject = $regions->isRegionalObject($objectFormat);

		if(!$this->isObjectForm() && $isRegionalObject) {
			$object[$regionsField] = 
				array_intersect($object[$regionsField], $regions->getAllowedRegionsIds());
		}

		$permit = $regions->checkRegionAccess($objectFormat, $object) || !$this->isObjectForm();

		if($permit && $isRegionalObject && $regions->isRegionalObject($exObjectFormat)) {
			$regionFilter = $regions->findByRegion($object[$regionsField]);

			if(strlen($table['exWhere'])) {
				$table['exWhere'] .=  " AND ";
			}

			$table['exWhere'] .= $regionFilter;
		} elseif(!$permit) {
			$this->fireError();
		}
	}

	function getRegions(){
		return PXMultiRegions::getInstance();
	}

	function fireError($message='Доступ к этому объекту/действию запрещен !') {
		$app = PXRegistry::getApp();
		if(!($app->isAdminEngine() && !$app->isEngineInstanceof("AdminJSON"))) {
			return;
		}

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
