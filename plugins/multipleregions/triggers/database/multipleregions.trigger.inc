<?php

use PP\Lib\Http\Response;

class PXTriggerDatabaseMultipleregions extends PXAbstractDatabaseTrigger
{
	private $isRegionalCloning = false;
	private $parentObject = null;

	public function OnBeforeAddObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues)
	{
		$regions = $this->getRegions();

		$isRegionalObject = $regions->isRegionalObject($format);
		$allowedRegionsIds = $regions->getAllowedRegionsIds();
		$rid = $regions->getRid();

		if (!$this->isRegionalCloning && $this->parentObject && !$isRegionalObject) {
			PXMultipleRegionsHelper::simpleSetDbValues('parent', $this->parentObject['id'], $dbFields, $dbValues);
			return true;
		}

		if ($isRegionalObject && empty($allowedRegionsIds) && $rid) {
			PXMultipleRegionsHelper::simpleSetDbValues(PXMultiRegions::REGION_MARK, "{{$rid}}", $dbFields, $dbValues);
			return true;
		}

		$permit = $regions->checkRegionAccess($format, $object) && $regions->checkRegionAccessToParent($format, $object);

		if ($permit) {
			$reflex_field = PXMultipleRegionsReflexer::REFLEX_FIELD;

			if (isset($object[$reflex_field])) {
				PXMultipleRegionsHelper::simpleSetDbValues($reflex_field, $object[$reflex_field], $dbFields, $dbValues);

				if (isset($object['allowed']) && is_array($object['allowed'])) {
					PXMultipleRegionsHelper::simpleSetDbValues('allowed', serialize($object['allowed']), $dbFields, $dbValues);
				}
			}

			$regions->addRegionByDefault($format, $dbFields, $dbValues, $object);
		} else {
			$this->fireError();
		}

		return $permit;
	}


	public function OnBeforeBuildTree($db, $format, &$table, $parentName)
	{
		$regions = $this->getRegions();

		if ($regions->isFederal() && !$regions->getRid()) { //Federal redactors can see original tree without reflex inserts when choosing "Show all regions"
			return;
		}

		$refl = @array_flip(GetColFromTableWithIndexs($table, PXMultipleRegionsReflexer::REFLEX_FIELD));

		foreach ($table as $id => $t) {
			if (isset($t['parent']) && isset($refl[$t['parent']])) {
				$table[$id]['parent'] = $refl[$t['parent']];
			}
		}
	}


	public function OnBeforeModifyObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB)
	{
		$regions = $this->getRegions();

		$isRegionalObject = $regions->isRegionalObject($format);
		$allowedRegionsIds = $regions->getAllowedRegionsIds();
		$regions_field = PXMultiRegions::REGION_MARK;

		if ($isRegionalObject && empty($allowedRegionsIds)) {
			PXMultipleRegionsHelper::simpleSetDbValues(PXMultiRegions::REGION_MARK,
				PXMultipleRegionsHelper::toString($objectInDB[$regions_field]), $dbFields, $dbValues);

			return true;
		}

		$permit = $regions->checkRegionAccess($format, $objectInDB) && $regions->checkRegionAccessToParent($format, $objectInDB);

		// клонирование возможно только из админики
		// рекурсивное клонирование заперещено
		if (!$permit && $isRegionalObject && PXRegistry::getApp()->isAdminEngine()) {

			if ($this->isRegionalCloning) {
				FatalError("Recusive clone detected!");
			}

			$this->isRegionalCloning = true; //flag initalize regional clone

			$cloner = new PXPublicRegionObjectCloner($format, $object, $objectInDB, $this);
			$nid = $cloner->cloneObject($allowedRegionsIds);

			$this->isRegionalCloning = null;

			$request = PXRegistry::getRequest();

			if ($this->isObjectForm()) {
				Response::getInstance()->redirect("popup.phtml?area={$request->getArea()}&format={$format->id}&id={$nid}&action=main");
			} else {
				$hasBeenCloned = true; //if not redirected to popup after clone, do not fire error !
			}
		}

		if ($isRegionalObject) {
			if ($permit) {
				if ($this->isRegionalCloning && isset($object['sys_order']) && is_numeric($object['sys_order']) && $object['sys_order'] > 0) {
					PXMultipleRegionsHelper::simpleSetDbValues('sys_order', $object['sys_order'], $dbFields, $dbValues);
				}

				$regions->addRegionByDefault($format, $dbFields, $dbValues, $object);
			} elseif (empty($hasBeenCloned)) {
				$this->fireError();
			}
		}

		# attempt to change is not a regional object whose parent is hidden by the regional mask
		if (!$isRegionalObject && !$permit) {
			$this->fireError();
		}

		return $permit;
	}

	public function onBeforeCloneObject($db, $format, &$object, $donor)
	{
		$regions = $this->getRegions();
		$isRegionalObject = $regions->isRegionalObject($format);

		// When cloning regional objects through standard cloning purify the regional field
		if ($isRegionalObject && !$this->isRegionalCloning) {
			$object[PXMultipleRegionsReflexer::REFLEX_FIELD] = null;
			$object[PXMultiRegions::REGION_MARK] = [];
		}

		return true;
	}

	public function OnBeforeDeleteObject(&$db, &$objId, &$format, &$objectInDB)
	{
		$regions = $this->getRegions();

		$permit = $regions->checkRegionAccess($format, $objectInDB) && $regions->checkRegionAccessToParent($format, $objectInDB);

		if (!$permit && $regions->isRegionalObject($format)) {
			$cloner =
				new PXPublicRegionObjectCloner($format, $object, $objectInDB, $this);

			$cloner->deleteObject($regions->getAllowedRegionsIds());
		} elseif (!$permit) {
			$this->fireError();
		}

		return $permit;
	}

	public function OnDeleteObject(&$db, &$objId, &$format, &$objectInDB)
	{
	}

	# fixme remove duplicate code
	public function OnBeforeModifyLinks(&$db, &$objId, &$formatId, &$links, $linkedFormat, &$objectInDB, $reference)
	{
		$regions = $this->getRegions();
		$format = PXRegistry::getApp()->types[$formatId];

		$isRegionalObject = $regions->isRegionalObject($format);
		$allowedRegionsIds = $regions->getAllowedRegionsIds();
		$regions_field = PXMultiRegions::REGION_MARK;

		if ($isRegionalObject && empty($allowedRegionsIds)) {
			PXMultipleRegionsHelper::simpleSetDbValues(PXMultiRegions::REGION_MARK,
				PXMultipleRegionsHelper::toString($objectInDB[$regions_field]), $dbFields, $dbValues);

			return true;
		}

		$permit = $regions->checkRegionAccess($format, $objectInDB) && $regions->checkRegionAccessToParent($format, $objectInDB);

		// клонирование возможно только из админики
		// рекурсивное клонирование заперещено
		if (!$permit && $isRegionalObject && PXRegistry::getApp()->isAdminEngine()) {

			if ($this->isRegionalCloning) {
				FatalError("Recusive clone detected!");
			}

			$this->isRegionalCloning = true; //flag initalize regional clone

			$cloner = new PXPublicRegionObjectCloner($format, $objectInDB, $objectInDB, $this);
			$nid = $cloner->cloneObject($allowedRegionsIds);

			$this->isRegionalCloning = null;

			$db->modifyLinks($reference, $format->id, $nid, $links);

			$request = PXRegistry::getRequest();
			Response::getInstance()->redirect("popup.phtml?area={$request->getArea()}&format={$format->id}&id={$nid}&action=links");
		}

		return $permit;
	}

	public function OnModifyLinks(&$db, &$objId, &$formatId, &$links, $linkedFormat)
	{ // simple
	}

	public function OnBeforeMoveObject(&$db, &$objId, &$format, &$objectInDB)
	{
		$regions = $this->getRegions();

		#fixme
		$permit = $regions->checkRegionAccess($format, $objectInDB) && $regions->checkRegionAccessToParent($format, $objectInDB);

		if (!$permit && $regions->isRegionalObject($format)) {
			$this->isRegionalCloning = true;

			$cloner = new PXPublicRegionObjectCloner($format, $objectInDB, $objectInDB, $this);
			$cloner->moveObject($regions->getAllowedRegionsIds());

			$this->isRegionalCloning = null;
			$permit = true; //Мы клонировали объект, $objectInDB теперь содержит клон, который необходимо позволить-таки передвинуть.
		} elseif (!$permit) {
			$this->fireError();
		}

		return $permit;
	}

	#todo cache sql-queries
	public function onBeforeCheckForParent($db, $format, &$object, &$parentObject)
	{

		$regions = $this->getRegions();
		$types = PXRegistry::getApp()->types;

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

			if (!empty($parentObject)) {
				$this->parentObject = $parentObject = reset($parentObject);
				return true;
			}

			# not founded parent. Fatal Error
			return false;
		}

		#standart answer for skip.
		return null;
	}

	public function OnBeforeGetDataBuildQuery($db, $format, &$mode, &$selector, &$where, &$order, &$limit)
	{
		$regions = $this->getRegions();

		if (strlen($where)) {
			$reflexer = new PXMultipleRegionsReflexer($regions, $where, $format);
			$where = $reflexer->getWhere();
		}

		if ($regions->isRegionalObject($format)) {
			$regionFilter = $regions->findByRegion();
			$where = strlen($where) ? "($where) AND $regionFilter" : $regionFilter;

			if ($mode != DB_SELECT_COUNT) {
				$reflex_field = PXMultipleRegionsReflexer::REFLEX_FIELD;
				$selector .= ", {$format->id}.{$reflex_field}";
			}
		}
	}

	public function isObjectForm()
	{
		return PXRegistry::getApp()->isAdminEngine() && PXRegistry::getRequest()->issetVar(PXMultiRegions::REGION_MARK . "_objectform");
	}

	public function OnBeforeNormalizePathBuildQuery($objectFormat, $exObjectFormat, &$table, $object = null)
	{
		$regions = $this->getRegions();
		$regionsField = PXMultiRegions::REGION_MARK;

		if ($regions->isRegionalObject($objectFormat) && $regions->isRegionalObject($exObjectFormat)) {
			//TODO: check me again, carefully !
			if (isset($object[$regionsField]) && empty($object[$regionsField]) && isset($object['id'])) {
				$object = PXRegistry::getDB()->getObjectById($objectFormat, $object['id']);
			}

			if (!isset($object[$regionsField])) {
				$object[$regionsField] = [];
			}

			$object[$regionsField] =
				array_intersect($object[$regionsField], $regions->getAllowedRegionsIds());

			$regionFilter = $regions->findByRegion($object[$regionsField]);

			if (strlen($table['exWhere'])) {
				$table['exWhere'] .= " AND ";
			}

			$table['exWhere'] .= $regionFilter;
		}
	}


	public function getRegions()
	{
		return PXMultiRegions::getInstance();
	}

	public function fireError($message = 'Доступ к этому объекту/действию запрещен !')
	{
		$app = PXRegistry::getApp();
		if (!$app->isAdminEngine()) {
			return;
		}

		$request = PXRegistry::getRequest();
		$response = Response::getInstance();
		$returnTo = $request->GetHttpReferer();

		if (empty($returnTo)) {
			$returnTo = '/admin';
		}

		$response->dontCache();

		if ($app->isEngineInstanceof("AdminJSON")) {
			$response->setContentType('text/javascript');
			$response->send(json_encode(PXEngineJSON::toError($message, "encode to utf8")));
		} else {
			$response->send(<<<JS
				<script type="text/javascript">
					alert('{$message}');
					window.location.href = '{$returnTo}';
				</script>
JS
			);
		}
		exit;
	}
}
