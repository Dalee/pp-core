<?php
abstract class PXAbstractDatabaseTrigger extends PXAbstractTrigger {

	//All ObBefore* triggers MUST return TRUE, otherwise object will not be added/modifyed/deleted !
	//It can be used for additional content checks in order to forbid addition/modification/deletion of object. 
	function OnBeforeAddObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues) {
		return true;
	}
	
	function OnAddObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues) {
	}
	
	function OnBeforeModifyObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB) {
		return true;
	}
	
	function OnModifyObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB) {
	}

	function OnBeforeDeleteObject(&$db, &$objId, &$format, &$objectInDB) {
		return true;
	}
	
	function OnDeleteObject(&$db, &$objId, &$format, &$objectInDB) {
	}
	
	function OnBeforeModifyLinks(&$db, &$objId, &$formatId, &$arLinks, $linkedFormat, &$objectInDB, $reference) {
		return true;
	}
	
	function OnModifyLinks(&$db, &$objId, $formatId, &$arLinks, $linkedFormat) { // simple
	}
	
	function OnBeforeMoveObject(&$db, &$objectId, &$format, &$objectInDB){
		return true;
	}
	
	function OnMoveObject(&$db, &$objectId, &$format, &$objectInDB){
	}
	
	//Triggers for low-level sql query modifications before do it
	function OnBeforeGetDataBuildQuery($db, $format, &$mode, &$selector, &$where, &$order, &$limit){
		return true;
	}
	
	function OnGetDataBuildQuery($db, $format, &$mode, &$query, &$selector, &$where, &$order, &$limit){
	}
	
	function OnBeforeNormalizePathBuildQuery($objectFormat, $exObjectFormat, &$table, $object=null) {
	}

	function OnBeforeBuildTree($db, $format, &$table, $parentName) {
	}

	# return true if parent founded
	#        false if not founded
	# for disable return null (nothing)
	function onBeforeCheckForParent($db, $format, &$object, &$parentObject) {
	}

	function onBeforeCloneObject($db, $format, &$object, $donor) {
		return true;
	}
}

?>
