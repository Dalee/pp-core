<?php

namespace PP\Module;

/**
 * Class ObjectsModule.
 *
 * @package PP\Module
 */
class ObjectsModule extends AbstractModule
{
	public $formats;
	public $object;

	public function __construct($area, $settings)
	{
		parent::__construct($area, $settings);

		$this->formats = [];

		if (isset($settings['format'])) {
			if (is_array($settings['format'])) {
				$this->formats = $settings['format'];
			} else {
				$this->formats[] = $settings['format'];
			}
		}
	}

	public function adminIndex()
	{
		$rSid = $this->request->getSid();

		if (!$this->indexSetMenu($rSid)) {
			return;
		}

		$this->datatype = $this->getDatatype('sid');
		$format = $this->datatype->id;

		$rView = $this->request->getVar($format . '_view');

		if ($rView === 'plain' || $this->datatype->struct === 'plain') {
			$this->indexAppendTable();
		} else {
			$this->indexAppendTree();
		}
	}

	public function indexSetMenu($rSid)
	{
		$types = [];
		$canProceed = false;
		foreach ($this->app->types as $k => $v) {
			if ((count($this->formats) == 0 || in_array($k, $this->formats)) && $this->user->can('admin', $v)) {
				$types[$k] = $v->title;
				$canProceed = $canProceed || $rSid == $k;
			}
		}

		uasort($types, [$this, '__sortTypes']);

		$this->layout->assignKeyValueList('INNER.0.0', $types, $rSid);
		return $canProceed;
	}

	public function indexAppendTable()
	{
		$table = new \PXAdminTableObjects($this->datatype);
		$table->addToParent('INNER.1.0');

		$cId = $this->request->getCid();
		$table->showChildren('cid');

		if ($cId && $table->has($cId)) {
			$this->indexAppendChild($table->get($cId));
		}
	}

	public function indexAppendChild($cidObject)
	{
		$format = $this->datatype->id;
		$rqCid = $this->request->getCid();

		$this->layout->setThreeColumns();
		$this->layout->setGetVarToSave('cid', $rqCid);

		$allowedChilds = $this->app->GetAllowedChildsKeys($format, $cidObject);

		if (count($allowedChilds)) {
			foreach ($allowedChilds as $childFormat) {
				if ($this->app->types[$childFormat]->struct == 'tree' && $this->request->GetVar($childFormat . '_view') != 'plain') {
					$objects = new \PXAdminTreeObjects($childFormat, $rqCid, 'ByParent');

				} else {
					$objects = new \PXAdminTableObjects($childFormat, $rqCid, 'ByParent');
				}

				$objects->setControlParent($rqCid);
				$objects->addToParent('INNER.2.0');
			}

		} else {
			$this->layout->notSetAllowedChilds('INNER.2.0', $format, $rqCid);
		}
	}


	public function indexAppendTree()
	{
		$objects = new \PXAdminAjaxTreeObjects($this->datatype);
		$objects->addToParent('INNER.1.0');
	}

	public function adminPopup()
	{
		$this->datatype = $this->getDatatype();
		$object = $this->popupGetObject();

		if (!$this->user->isAdmin() || empty($object)) {
			return $this->layout->assignError('OUTER.CONTENT', 'Нет доступа');
		}

		$action = $this->request->getAction();

		$this->layout->setGetVarToSave('id', $this->request->getId());
		$this->layout->setGetVarToSave('format', $this->datatype->id);
		$this->layout->setGetVarToSave('action', $action);

		$counts = [];
		$possibleFormats = $this->datatype->childTypes();

		foreach ($this->datatype->allowedChildTypes($object) as $allowedFormat => $childType) {
			$counts[$allowedFormat] = $this->db->getObjectsByParent($childType, NULL, $object['id'], DB_SELECT_COUNT);
		}

		$this->db->LoadDirectoriesByType($this->datatype, $object);

		// warn about parent if need
		if (!empty($this->datatype->parent) && empty($object['parent']) && $this->datatype->struct != 'tree') {
			$parentDatatype = $this->app->types[$this->datatype->parent];

			\PXDecorativeWidgetsCollection::addToCollection(
				"<em class='warning'>Внимание!</em> " .
				"Объект типа '<b>{$this->datatype->title}</b>' может не быть создан вне контекста объекта '<b>{$parentDatatype->title}</b>'.<br/>" .
				"Рекомендуется не использовать данный функционал для создания объектов данного типа.",
				'PXAdminForm',
				'notices',
				\PXAdminForm::NOTICES_CONTENT
			);
		}

		$form = new \PXAdminForm($object, $this->datatype);
		$form->setDisabledStatus($this->popupCheckAccess($object));
		$form->setAction($this->request->getAction());
		$form->setArea($this->getArea());
		$form->setChildren($possibleFormats, $counts);
		$form->setLinks($this->popupLinks($object));
		$form->setTitle($this->popupSetTitle($object));
		$form->getForm();
		$form->setDisabledStatus(false); //release global NLAbstractHTMLForm lock!
	}

	public function &getDatatype($varName = 'format')
	{
		$rFormat = $this->request->getVar($varName);
		if (!isset($this->app->types[$rFormat])) {
			FatalError('Undefined datatype ' . $rFormat);
		}
		return $this->app->types[$rFormat];
	}

	public function popupGetObject()
	{
		$rId = $this->request->getId();
		$object = [];

		if ($rId == 0) {
			$object = $this->app->initContentObject($this->datatype->id);
			$rObject = $this->request->getContentObject($this->datatype);

			foreach ($rObject as $k => $v) {
				if (array_key_exists($k, $object) && is_null($object[$k])) {
					$object[$k] = $v;
				}
			}
		} else {
			if (!sizeof($object = $this->db->getObjectById($this->datatype, $rId))) {
				FatalError('Объект не существует !');
			}

			if (!$this->user->can('admin', $this->datatype, $object)) {
				$object = [];
			}
		}

		if (isset($object['sys_owner'])) {
			$tmp = $this->db->getObjectById($this->app->types['suser'], $object['sys_owner']);
			$object['ownerlogin'] = isset($tmp['title']) ? $tmp['title'] : '';
		}

		return $object;
	}

	public function popupCheckAccess($object)
	{
		if (!$this->user->can(['write', (($this->request->getId() > 0) ? 'modify' : 'add')], $this->datatype, $object)) {
			return true;
		}
	}

	public function popupSetTitle($object)
	{
		$title = '';
		if ($this->request->getId() > 0) {
			if (isset($object) && isset($object['title'])) {
				$title = '&laquo;' . mb_substr(strip_tags($object['title']), 0, 32) . '&raquo; &#8212; ';
			}
			$title .= ' Редактирование';
		} else {
			$title = 'Добавление';
		}
		$title .= ' объекта формата &laquo;' . strip_tags($this->datatype->title) . '&raquo;';
		return $title;
	}

	public function popupLinks($object)
	{
		$db =& $this->db;
		$app =& $this->app;
		$datatype =& $this->datatype;
		$request =& $this->request;
		$layout =& $this->layout;

		$linksParam = [];

		$refFilters = (array)$request->GetVar('filters', []);
		foreach ($datatype->references as $k => $reference) {
			if ($reference->hidden) {
				continue;
			}
			$refDatatype = $app->types[$k];

			$existingLinks = $db->getLinks($reference, $datatype->id, $object['id']);

			$onPage = $app->getProperty('LINKS_ON_PAGE', 10);
			$currentPage = $request->getVar($k . '_page', 1);
			$layout->setGetVarToSave($k . '_page', $currentPage);

			$onlyExistingLinks = $request->GetVar($k . '_exist', $reference->byDefault);
			$layout->setGetVarToSave($k . '_exist', $onlyExistingLinks);

			$filteredWhere = $this->applyFiltersToWhere($refFilters, $refDatatype);
			$trueCond = $db->TrueStatusString() . '=' . $db->TrueStatusString();
			$falseCond = $db->TrueStatusString() . '=' . $db->TrueStatusString(false);

			if ($onlyExistingLinks) {
				if (count($existingLinks)) {
					$existingLinksWhere = $k . '.id IN ( ' . join(',', array_keys($existingLinks)) . ' ) ';
				} else {
					$existingLinksWhere = $falseCond;
				}
				[$possibleLinks, $pagerCount, $overallCount] = $this->_getLinksData($refDatatype, $existingLinksWhere . $filteredWhere, $trueCond, $onPage, $currentPage);
			} else {
				$mainWhere = $trueCond; //No stupid NULL

				if (!empty($reference->filterFrom) && $reference->from == $k) {
					$mainWhere = '(' . $db->parseWhereTemplate($reference->filterFrom, $object, $datatype) . ')';
				}

				if (!empty($reference->filterTo) && $reference->to == $k) {
					$mainWhere = '(' . $db->parseWhereTemplate($reference->filterTo, $object, $datatype) . ')';
				}

				if (!empty($reference->restrictBy)) {
					$mainWhere .= ' AND ';
					$mainWhere .= $k . '.' . $reference->restrictBy . " = '" . $db->escapeString($object[$reference->restrictBy]) . "'";
					$mainWhere = '(' . $mainWhere . ')';
				}
				[$possibleLinks, $pagerCount, $overallCount] = $this->_getLinksData($refDatatype, $mainWhere . $filteredWhere, $mainWhere, $onPage, $currentPage);
				if ($datatype->id == $k) {
					unset($possibleLinks[$object['id']]);
				}
			}

			$linksParam[$k] = [
				"reference" => $reference,
				"formatTo" => $refDatatype,
				"links" => $existingLinks,
				"pLinks" => $possibleLinks,
				"objectsOnPage" => $onPage,
				"page" => $currentPage,
				"count" => $pagerCount,
				"fullCount" => $overallCount,
				"onlyExistingLinks" => $onlyExistingLinks,
				"filters" => $refFilters,
			];
		}
		return $linksParam;
	}

	public function _getLinksData($dType, $where, $whereAllCount, $onPage, $cPage)
	{
		$pLinks = $this->db->getObjectsByWhereLimited($dType, NULL, $where, $onPage, $onPage * ($cPage - 1));
		$pCount = $this->db->getObjectsByWhere($dType, NULL, $where, DB_SELECT_COUNT);
		$oCount = $this->db->getObjectsByWhere($dType, NULL, $whereAllCount, DB_SELECT_COUNT);

		return [$pLinks, $pCount, $oCount];
	}

	public function adminAction()
	{
		$this->datatype =& $this->getDatatype();

		$action = $this->_route();
		switch ($action) {
			default: //FIXME: default main action it is right behaviour ?
			case 'main':
				$this->actionMain();
				break;

			case 'children':
				$this->actionChildren();
				break;

			case 'remove':
				$this->actionRemove();
				break;

			case 'directup':
				$this->actionUpContentObject();
				break;

			case 'directdown':
				$this->actionDownContentObject();
				break;

			case 'directmove':
				$this->actionMoveContentObject();
				break;

			case 'directremove':
				$this->actionRemoveDirect();
				break;

			case 'directstatus':
				$this->actionChangeStatus();
				break;

			case 'clone':
				$this->actionCloneContentObject();
				break;

			case 'links':
				$this->actionModifyLinks();
				break;

			case 'auditlog':
				//do nothing. FIXME: make actions configurable (like in multioperations module), not statically written here !
				break;
		}
		return $this->nextLocation;
	}

	protected function getArea()
	{
		return 'objects';
	}

	public function _route()
	{
		$rAction = $this->request->getAction();
		$this->nextLocation = 'popup.phtml?area=' . $this->getArea() . '&format=' . $this->datatype->id . '&action=' . $rAction;
		return $rAction;
	}

	public function actionMain()
	{
		$object = $this->request->getContentObject($this->datatype);

		if ($object['id']) {
			$this->db->modifyContentObject($this->datatype, $object, true); // Preserve values with displaytype HIDDEN or STATIC
			$this->returnToId($object['id']);

		} else {
			$id = $this->db->addContentObject($this->datatype, $object);
			$this->returnToId($id);
		}
	}

	public function actionCloneContentObject()
	{
		$donor = $this->request->getContentObject($this->datatype);

		if (!is_numeric($donor['id']) || !$donor['id']) {
			FatalError('Клонировать можно только существующие объекты');
		}

		$cloneId = $this->db->cloneContentObject($this->datatype, $donor['id']);
		$this->returnToId($cloneId, 'main');
	}

	public function actionChildren()
	{
		if (!count($this->datatype->childs)) {
			FatalError("В этом разделе невозможно назначать потомков");
		}

		$object = $this->request->getObjectSysVars($this->datatype, [OBJ_FIELD_CHILDREN]);

		if ($object['id']) {
			$this->db->modifyObjectSysVars($this->datatype, $object);
			$this->returnToId($object['id']);

		} else {
			$this->returnToReferer();
		}
	}

	public function actionRemove()
	{
		if ($this->request->getAck()) {
			$this->db->deleteContentObject($this->datatype, $this->request->getID());
		}

		closeAndRefresh();
	}

	public function actionRemoveDirect()
	{
		$this->db->deleteContentObject($this->datatype, $this->request->getId());
		$this->returnToReferer();
	}

	public function applyFiltersToUri($filters)
	{
		foreach ($filters as $dt => $fields) {
			if (is_array($fields) && count($fields) > 0) {
				foreach ($fields as $field => $filter) {
					if (empty($filter)) continue;
					$this->nextLocation .= '&filters[' . $dt . ']' . '[' . $field . ']=' . rawurlencode($filter);
				}
			}
		}
	}

	public function applyFiltersToWhere($refFilters, $refDatatype)
	{
		$filterOnWhere = NULL;
		if (isset($refFilters[$refDatatype->id]) &&
			is_array($refFilters[$refDatatype->id]) &&
			count($refFilters[$refDatatype->id]) > 0
		) {
			$applied = 0;
			$filterOnWhere = ' AND (';
			foreach ($refFilters[$refDatatype->id] as $field => $filter) {
				if (empty($filter)) continue;
				$applied++;
				$this->layout->setGetVarToSave('filters[' . $refDatatype->id . '][' . $field . ']', $filter);
				//apply simple filter logic, ex. : ^search string$
				$modifiers = P_LEFT | P_RIGHT;
				if (substr($filter, 0, 1) == '^') {
					$modifiers &= P_RIGHT;
					$filter = substr($filter, 1);
				}
				if (substr($filter, -1) == '$') {
					$modifiers &= P_LEFT;
					$filter = substr($filter, 0, -1);
				}

				// build where
				$filterOnWhere .= $refDatatype->id . '.' . $this->db->escapeString($field) . $this->db->LIKE($filter, $modifiers) . " AND ";
			}

			if (!$applied) {
				$filterOnWhere = NULL;
			} else {
				$filterOnWhere = substr($filterOnWhere, 0, -4); //remove last AND or OR
				$filterOnWhere .= ')';
			}
		}
		return $filterOnWhere;
	}

	public function appendLinksExistFlag()
	{
		foreach ($this->request->GetAllPostData() as $k => $v) {
			if (is_string($k) && strstr($k, '_exist') == '_exist') {
				$this->nextLocation .= "&{$k}={$v}";
			}
		}
	}

	public function returnToId($id, $action = null)
	{
		$this->nextLocation .= '&id=' . $id;

		if (is_string($action)) {
			$this->nextLocation .= '&action=' . $action;
		}
	}

	public function returnToReferer()
	{
		$this->nextLocation = $this->request->GetReferer();
	}

	public function actionUpContentObject()
	{
		$this->db->upContentObject($this->datatype, $this->request->getId());
		$this->returnToReferer();
	}

	public function actionDownContentObject()
	{
		$this->db->downContentObject($this->datatype, $this->request->getId());
		$this->returnToReferer();
	}

	public function actionMoveContentObject()
	{
		$this->db->moveContentObject($this->datatype, $this->request->getId(), $this->request->getVar('shift'));
		$this->returnToReferer();
	}

	public function actionChangeStatus()
	{
		$object = $this->db->getObjectById($this->datatype, $this->request->getId());
		$object['status'] = !$object['status'];
		$this->db->modifyContentObject($this->datatype, $object);

		$this->returnToReferer();
	}

	public function actionModifyLinks()
	{
		$id = $this->request->getId();
		foreach ($this->datatype->references as $reference) {
			if ($reference->hidden) {
				continue;
			}
			$ll = $this->_makeLinkedListFrom($reference);
			$this->db->ModifyLinks($reference, $this->datatype->id, $id, $ll, false);
		}

		if (count($filters = (array)$this->request->GetVar('filters', []))) {
			$this->applyFiltersToUri($filters);
		}
		$this->appendLinksExistFlag();
		$this->returnToId($id);
	}

	public function _makeLinkedListFrom($ref)
	{
		$linkedList = [];
		foreach ($this->request->getLinks($ref) as $values) {
			foreach ($values as $id => $data) {
				$node = &$linkedList[$id];
				if (isset($linkedList[$id])) {
					while (!is_null($node = &$node['next'])) ;
				}
				$node = $data;
				$node['next'] = NULL;
			}
		}
		return $linkedList;
	}

	public function __sortTypes($a, $b)
	{
		return strcoll($a, $b);
	}
}
