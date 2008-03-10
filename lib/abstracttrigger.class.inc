<?
class PXAbstractTrigger {
	function PXAbstractTrigger() {
		// none action
	}

	function OnAddObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues) {
	}

	function OnModifyObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB) {
	}

	function OnDeleteObject(&$db, &$objId, &$format, &$objectInDB) {
	}

	function OnModifyLinks(&$db, &$objId, &$formatId) { // simple
	}

}

?>
