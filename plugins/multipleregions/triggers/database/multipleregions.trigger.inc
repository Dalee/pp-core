<?

class PXTriggerDatabaseMultipleregions extends PXAbstractDatabaseTrigger {
	function OnBeforeAddObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues) {
		$regions = $this->getRegions();

		$permit = $regions->checkRegionAccess($format, $object) && $regions->checkRegionAccessToParent($format, $object);

		if($permit) {
			$reflex_field = PXMultipleRegionsReflexer::REFLEX_FIELD;

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
			$refl  = @array_flip(GetColFromTableWithIndexs($table, PXMultipleRegionsReflexer::REFLEX_FIELD));

			foreach($table as $id => $t)  {
				if(isset($t['parent']) && isset($refl[$t['parent']])) {
					$table[$id]['parent'] = $refl[$t['parent']];
				}
			}
		}
	}/*}}}*/

	function OnBeforeModifyObject(&$db, $objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB) {/*{{{*/
		$regions = $this->getRegions();

		$permit = $regions->checkRegionAccess($format, $objectInDB) && $regions->checkRegionAccessToParent($format, $objectInDB);

		if(!$permit && $regions->isRegionalObject($format)) {
			$cloner = 
				new PXPublicRegionObjectCloner($format, $object, $objectInDB, $dbFields, $dbValues, $this);

			$nid = $cloner->cloneObject($regions->getAllowedRegionsIds());

			$request = PXRegistry::getRequest();
		
			if(PXRegistry::getApp()->isAdminEngine() && $request->issetVar(PXMultiRegions::REGION_MARK . "_objectform")) {
				PXResponse::getInstance()->redirect("popup.phtml?area={$request->getArea()}&format={$format->id}&id={$nid}&action=main");
			} else {
				$hasBeenCloned = true; //if not redirected to popup after clone, do not fire error !
			}
		}

		if($permit) {
			$regions->addRegionByDefault($format, $dbFields, $dbValues);
		} elseif(empty($hasBeenCloned)){
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

			$cloner->deleteObject($regions->getAllowedRegionsIds()); //FIXME, i am broken in multioperations !
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

			$cloner->moveObject($regions->getAllowedRegionsIds());
		} elseif(!$permit) {
			$this->fireError();
		}

		return $permit;
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
				$selector .= ", {$reflex_field}";
			}
		}
	}

	function OnBeforeNormalizePathBuildQuery($objectFormat, $exObjectFormat, &$table, $object) {
		$regions = $this->getRegions();

		$permit = $regions->checkRegionAccess($objectFormat, $object);

		if($permit && $regions->isRegionalObject($objectFormat) && $regions->isRegionalObject($exObjectFormat)) {
			$regionFilter = $regions->findByRegion($object[PXMultiRegions::REGION_MARK]);

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
		if(!PXRegistry::getApp()->isAdminEngine()) {
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
