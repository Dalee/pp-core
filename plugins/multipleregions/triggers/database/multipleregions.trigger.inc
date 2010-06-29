<?
class PXTriggerDatabaseMultipleregions extends PXAbstractDatabaseTrigger {
	function OnBeforeAddObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues) {
		$regions = $this->getRegions();
		$regions->addRegionByDefault($format, $dbFields, $dbValues);
		$permit = $regions->checkRegionAccessToParent($format, $object);
		if(!$permit){
			$this->fireError();
		}
		return $permit;
	}

	function OnBeforeModifyObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB) {
		$regions = $this->getRegions();

		$permit = $regions->checkRegionAccess($format, $objectInDB) && $regions->checkRegionAccessToParent($format, $objectInDB);
		
		if($permit){
			$regions->addRegionByDefault($format, $dbFields, $dbValues);
		} else {
			$this->fireError();
		}

		return $permit;
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
		
		if($regions->isRegionalObject($format)){
			$regionFilter = $regions->findByRegion();
			$where = strlen($where) ? "($where) AND $regionFilter" : $regionFilter;
		}
	}
	
	function OnBeforeNormalizePathBuildQuery($objectFormat, $exObjectFormat, &$table) {
		$regions = $this->getRegions();
		
		if($regions->isRegionalObject($objectFormat) && $regions->isRegionalObject($exObjectFormat)) {
			$regionFilter = $regions->findByRegion();
	
			strlen($table['exWhere']) && $table['exWhere'] .= " and ";
			$table['exWhere'] .= $regionFilter;
		}
	}

	function getRegions(){
		return PXMultiRegions::getInstance();
	}

	function fireError(){
		$request  = PXRegistry::getRequest();
		$returnTo = $request->GetHttpReferer();
		
		if(empty($returnTo)){
			$returnTo = '/admin';
		}

		echo <<<JS
			<script type="text/javascript">
				alert('Доступ к этому объекту/действию запрещен !');
				window.location.href = '{$returnTo}';
			</script>
JS;
		exit;
	}
}
?>
