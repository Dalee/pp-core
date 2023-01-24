<?php

use PP\Lib\Database\Driver\PostgreSqlDriver;

abstract class PXAbstractDatabaseTrigger extends PXAbstractTrigger
{

	//All ObBefore* triggers MUST return TRUE, otherwise object will not be added/modifyed/deleted !
	//It can be used for additional content checks in order to forbid addition/modification/deletion of object.
	public function OnBeforeAddObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues)
	{
		return true;
	}

	/**
  * @param int $objId
  * @param PXTypeDescription $format
  * @param array $object
  * @param array $dbFields
  * @param array $dbValues
  */
 public function OnAddObject(\PXDatabase|\PP\Lib\Database\Driver\PostgreSqlDriver &$db, &$objId, &$format, &$object, &$dbFields, &$dbValues)
	{
	}

	public function OnBeforeModifyObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB)
	{
		return true;
	}

	/**
  * @param int $objId
  * @param PXTypeDescription $format
  * @param array $object
  * @param array $dbFields
  * @param array $dbValues
  * @param array $objectInDB
  */
 public function OnModifyObject(\PXDatabase|\PP\Lib\Database\Driver\PostgreSqlDriver &$db, &$objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB)
	{
	}

	public function OnModifyObjectSysVars(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB)
	{
	}

	public function OnBeforeDeleteObject(&$db, &$objId, &$format, &$objectInDB)
	{
		return true;
	}

	public function OnDeleteObject(&$db, &$objId, &$format, &$objectInDB)
	{
	}

	public function OnBeforeModifyLinks(&$db, &$objId, &$formatId, &$arLinks, $linkedFormat, &$objectInDB, $reference)
	{
		return true;
	}

	public function OnModifyLinks(&$db, &$objId, &$formatId, &$arLinks, $linkedFormat)
	{ // simple
	}

	public function OnBeforeMoveObject(&$db, &$objectId, &$format, &$objectInDB)
	{
		return true;
	}

	public function OnMoveObject(&$db, &$objectId, &$format, &$objectInDB)
	{
	}

	//Triggers for low-level sql query modifications before do it
	public function OnBeforeGetDataBuildQuery($db, $format, &$mode, &$selector, &$where, &$order, &$limit)
	{
		return true;
	}

	public function OnGetDataBuildQuery($db, $format, &$mode, &$query, &$selector, &$where, &$order, &$limit)
	{
	}

	public function OnBeforeNormalizePathBuildQuery($objectFormat, $exObjectFormat, &$table, $object = null)
	{
	}

	public function OnBeforeBuildTree($db, $format, &$table, $parentName)
	{
	}

	# return true if parent founded
	#        false if not founded
	# for disable return null (nothing)
	public function onBeforeCheckForParent($db, $format, &$object, &$parentObject)
	{
	}

	public function onBeforeCloneObject($db, $format, &$object, $donor)
	{
		return true;
	}

	public function onCloneObject($db, $format, &$object, $donor)
	{
	}
}

