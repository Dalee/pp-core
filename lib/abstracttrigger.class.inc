<?
class PXAbstractTrigger {
	function PXAbstractTrigger() {
		// none action
	}

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
	
	function OnBeforeModifyLinks(&$db, &$objId, &$formatId, &$arLinks) {
		return true;
	}
	
	function OnModifyLinks(&$db, &$objId, &$formatId, &$arLinks) { // simple
	}
	
	function OnBeforeMoveObject(&$db, &$objectId, &$format, &$objectInDB){
		return true;
	}
	
	function OnMoveObject(&$db, &$objectId, &$format, &$objectInDB){
	}

}

?>
