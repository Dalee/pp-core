<?php
class PXModuleObjects extends PXModule {
	var $formats;

	function __construct($area, $settings) {
		parent::__construct($area, $settings);

		$this->formats = array();

		if (isset($settings['format'])) {
			if (is_array($settings['format'])) {
				$this->formats = $settings['format'];
			} else {
				$this->formats[] = $settings['format'];
			}
		}
	}

	function adminIndex() {
		$app     = $this->app;
		$layout  = $this->layout;
		$request = $this->request;

		$rSid = $request->getSid();

		$this->indexSetMenu($rSid);

		if (is_null($rSid)) {
			return;
		}

		$this->datatype =& $this->getDatatype('sid');
		$format = $this->datatype->id;

		$rView = $request->getVar($format.'_view');
		$this->layout->setGetVarToSave($format.'_view', $rView);


		if ($rView === 'plain' || $this->datatype->struct === 'plain' ) {
			$this->indexAppendTable();

		} else {
			$this->indexAppendTree();
		}

		$layout->assignControls('INNER.1.1', 0, array($format));
		$layout->assignContextMenu('INNER.1.0.CONTEXT', 0, array($format));
	}

	function indexSetMenu($rSid) {
		$types = array();
		foreach ($this->app->types as $k=>$v) {
			if (count($this->formats) == 0 || in_array($k, $this->formats)) {
				$types[$k] = $v->title;
			}
		}

		uasort($types, array($this, '__sortTypes'));

		$this->layout->assignKeyValueList('INNER.0.0', $types, $rSid);
	}

	function indexAppendTable() {
		$format = $this->datatype->id;

		$this->db->LoadDirectoriesByType($this->datatype);
		$count = & $this->_loadObjects(DB_SELECT_COUNT);

		$onPage      = $this->app->getProperty('CHILDREN_ON_PAGE', 20);
		$currentPage = $this->request->getVar($format.'_page', 1);
		$this->layout->setGetVarToSave($format.'_page', $currentPage);

		$currentPage  = $this->request->getVar($format.'_page', 1);
		$currentOrder = $this->request->getVar($format.'_order', $this->datatype->order);
		$this->layout->setGetVarToSave($format.'_order', $currentOrder);

		$currentPage = $currentPage > ceil($count/$onPage) ? ceil($count/$onPage) : $currentPage;

		$objects = & $this->_loadObjects(DB_SELECT_TABLE, $onPage, $onPage*($currentPage-1), $currentOrder);

		$table = new PXAdminTable($objects, $this->datatype, $this->layout->getData);
		$table->setPager($currentPage, $onPage, $count);
		$table->loadDropdownValues($this->db);

		$this->layout->append('INNER.1.0', $table->getTable());
	}

	function indexAppendTree() {
		$objects = & $this->_loadObjects(DB_SELECT_TREE);
		$this->layout->appendTableAsTree('INNER.1.0', $this->datatype->id, $objects, $this->request->getCid(), $this->request->getLeafStatus());
	}

	function &_loadObjects($mode, $limit = NULL, $offset = NULL, $order = NULL){
		if(is_null($limit) && is_null($offset)){
			$objects = $this->db->getObjects($this->datatype, NULL, $mode);
		} else {
			$objects = $this->db->getObjectsLimited($this->datatype, NULL, $limit, $offset, $mode, $order);
		}
		return $objects;
	}

	function adminPopup() {
		$layout  = $this->layout;
		$request = $this->request;
		$db      = $this->db;

		$this->datatype =& $this->getDatatype();

		$layout->setGetVarToSave('id',     $request->getId());
		$layout->setGetVarToSave('format', $this->datatype->id);
		$layout->setGetVarToSave('action', $request->getAction());

		$layout->setOuterForm('action.phtml', 'POST', 'multipart/form-data');

		$this->object = $this->popupGetObject();

		$this->form = new PXAdminObjectForm($this->datatype, $this->object);

		$this->popupSetMenu();
		$this->popupCheckAccess();

		$layout->assign('OUTER.LEFTCONTROLS',  $this->form->leftControls());
		$layout->assign('OUTER.RIGHTCONTROLS', $this->form->rightControls());
		$db->LoadDirectoriesByType($this->datatype, $this->object);

		switch ($request->getAction()) {
			case 'main':
				$this->popupMain();
				break;

			case 'children':
				$this->popupAllowedChildren();
				break;

			case 'remove':
				$this->popupRemove();
				break;

			case 'links':
				$this->popupLinks();
				break;
		}

		$this->popupSetTitle();
	}

	function &getDatatype($varName='format') {
		$rFormat = $this->request->getVar($varName);

		if (!isset($this->app->types[$rFormat])) {
			FatalError('Undefined datatype '.$rFormat);
		}

		return $this->app->types[$rFormat];
	}

	function popupGetObject() {
		$rId = $this->request->getId();

		if ($rId == 0) {
			$object  = $this->app->initContentObject($this->datatype->id);
			$rObject = $this->request->getContentObject($this->datatype);

			foreach($rObject as $k=>$v) {
				if(array_key_exists($k, $object) && is_null($object[$k])) {
					$object[$k] = $v;
				}
			}

		} else {
			$object = $this->db->getObjectById($this->datatype, $rId);
		}

		if(isset($object['sys_owner'])) {
			$tmp = $this->db->getObjectById($this->app->types['suser'], $object['sys_owner']);
			$object['ownerlogin'] = isset($tmp['title']) ? $tmp['title'] : '';
		}

		return $object;
	}

	function popupSetMenu() {
		$menuItems  = $this->form->getAllowedForms($this->user->level);
		$this->layout->setMenu($menuItems, $this->request->getAction(), 'action');
	}

	function popupCheckAccess(){
		if (!$this->user->can('write', $this->datatype, $this->object)) {
			$this->form->disable = true;
		}
	}

	function popupSetTitle() {
		$object =& $this->object;
		$title = '';

		if ($this->request->getId() > 0) {
			if(isset($object) && isset($object['title'])) {
				$title = '&laquo;'.substr(strip_tags($object['title']), 0, 32).'&raquo; &#8212; ';
			}

				$title .= ' Редактирование';

			} else {
				$title = 'Добавление';
			}

			$title .= ' объекта формата &laquo;'.strip_tags($this->datatype->title).'&raquo;';

		$this->layout->assignTitle($title);
	}

	function popupMain() {
		$tree     = & $this->popupLoadTree();
		$selfTree = & $this->popupLoadSelfTree();

		$this->layout->append('OUTER.CONTENT', $this->form->MainForm($tree, $selfTree, $this->datatype));
	}

	function &popupLoadTree(){
		if (($parentFormat = $this->datatype->parent) && strstr($this->datatype->fields['parent']->displayType, "DROPDOWN")) {
			$tree = $this->db->getObjects($this->app->types[$parentFormat], NULL, DB_SELECT_TREE);
		} else {
			$tree = new NLTree(array());
		}
		return $tree;
	}

	function &popupLoadSelfTree(){
		if ($this->datatype->struct == 'tree') {
			if (isset($this->datatype->fields['parent']) && !is_null($this->object['parent'])) {
				$selfTree = $this->db->getObjectsByParent($this->datatype, NULL, $this->object['parent'], DB_SELECT_TREE);
			} else {
				$selfTree = $this->db->getObjects($this->datatype, NULL, DB_SELECT_TREE);
			}
		} else {
			$selfTree = new NLTree(array());
		}
		return $selfTree;
	}

	function popupAllowedChildren() {
		$counts = array();
		$possibleFormats = $this->datatype->childTypes();

		foreach ($this->datatype->allowedChildTypes($this->object) as $allowedFormat=>$childType) {
			$counts[$allowedFormat] = $this->db->getObjectsByParent($childType, NULL, $this->object['id'], DB_SELECT_COUNT);
		}

		$this->layout->append('OUTER.CONTENT', $this->form->childrenForm($possibleFormats, $counts));
	}

	function popupRemove() {
		$this->layout->append('OUTER.CONTENT', $this->form->removeForm());
	}

	function popupLinks() {
		$db       =& $this->db;
		$app      =& $this->app;
		$datatype =& $this->datatype;
		$request  =& $this->request;
		$layout   =& $this->layout;
		$object   =& $this->object;

		foreach ($datatype->references as $k=>$reference) {
			$refDatatype   = $app->types[$k];

			$existingLinks = $db->getLinks($reference, $datatype->id, $object['id']);
			$fullList      = true;

			$onPage        = $app->getProperty('LINKS_ON_PAGE', 10);
			$currentPage   = $request->getVar($k.'_page', 1);
			$layout->setGetVarToSave($k.'_page', $currentPage);

			$onlyExistingLinks = (int)$request->GetVar($k.'_exist', false);
			$layout->setGetVarToSave($k.'_exist', $onlyExistingLinks);

			$where = NULL;

			if(!empty($reference->filterFrom) && $reference->from == $k) {
				$where = $db->parseWhereTemplate($reference->filterFrom, $object, $datatype);
			}

			if(!empty($reference->filterTo) && $reference->to == $k) {
				$where = $db->parseWhereTemplate($reference->filterTo, $object, $datatype);

			}

			if (!empty($reference->restrictBy)) {
				if(strlen($where)) {
					$where .= ' AND ';
				}

				$where .= $k . '.' . $reference->restrictBy." = '".$db->escapeString($object[$reference->restrictBy])."'";
			}

			if($onlyExistingLinks) {
				$count = 0;
				$possibleLinksKeys = array();

				foreach($existingLinks as $kk=>$vv) {
					if($count >= $onPage*($currentPage-1) && $count <$onPage*$currentPage) {
						$possibleLinksKeys[$kk] = $kk;
					}

					$count++;
				}

				$possibleLinks = $db->getObjectsByIdArray($refDatatype, NULL, $possibleLinksKeys);
				$count         = $db->getObjectsByWhere($refDatatype, NULL, $where, DB_SELECT_COUNT);

			} else {
				$possibleLinks = $db->getObjectsByWhereLimited($refDatatype, NULL, $where, $onPage, $onPage*($currentPage-1));
				$count         = $db->getObjectsByWhere($refDatatype, NULL, $where, DB_SELECT_COUNT);

				if ($datatype->id == $k) {
					unset($possibleLinks[$object['id']]);
				}
			}

			$layout->append('OUTER.CONTENT', $this->form->linksForm($reference, $refDatatype, $existingLinks, $possibleLinks, $onPage, $currentPage, $count, $layout, $onlyExistingLinks));
		}
	}

	function adminAction() {
		$this->datatype =& $this->getDatatype();

		$rAction = $request->getAction();

		$this->nextLocation = 'popup.phtml?area=objects&format='.$this->datatype->id.'&action='.$rAction;

		switch ($rAction) {
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

			case 'links':
				$this->actionModifyLinks();
				break;

			default:
				FatalError('Неизвестный системе <em>action</em>');
				break;
		}

		return $this->nextLocation;
	}

	function actionMain() {
		$object = $this->request->getContentObject($this->datatype);

		if ($object['id']) {
			$this->db->modifyContentObject($this->datatype, $object, true); // Preserve values with displaytype HIDDEN or STATIC
			$this->returnToId($object['id']);

		} else {
			$id = $this->db->addContentObject($this->datatype, $object);
			$this->returnToId($id);
		}
	}

	function actionChildren() {
		if (!count($this->datatype->childs)) {
			FatalError("В этом разделе невозможно назначать потомков");
		}

		$object = $this->request->getObjectSysVars($this->datatype, array(OBJ_FIELD_CHILDREN));

		if ($object['id']) {
			$this->db->modifyObjectSysVars($this->datatype, $object);
			$this->returnToId($object['id']);

		} else {
			$this->returnToReferer();
		}
	}

	function actionRemove() {
		if ($this->request->getAck()) {
			$this->db->deleteContentObject($this->datatype, $this->request->getID());
		}

		closeAndRefresh();
	}

	function actionRemoveDirect() {
		$this->db->deleteContentObject($this->datatype, $this->request->getId());
		$this->returnToReferer();
	}

	function returnToId($id) {
		$this->nextLocation .= '&id='.$id;
	}

	function returnToReferer() {
		$this->nextLocation = $this->request->GetReferer();
	}

	function actionUpContentObject() {
		$this->db->upContentObject($this->datatype, $this->request->getId());
		$this->returnToReferer();
	}

	function actionDownContentObject() {
		$this->db->downContentObject($this->datatype, $this->request->getId());
		$this->returnToReferer();
	}

	function actionMoveContentObject() {
		$this->db->moveContentObject($this->datatype, $this->request->getId(), $this->request->getVar('shift'));
		$this->returnToReferer();
	}

	function actionChangeStatus() {
		$object = $this->db->getObjectById($this->datatype, $this->request->getId());
		$object['status'] = ($object['status'] == true) ? false : true;
		$this->db->modifyContentObject($this->datatype, $object);

		$this->returnToReferer();
	}

	function actionModifyLinks() {
		$id = $this->request->getId();

		foreach ($this->datatype->references as $reference) {
			$this->db->ModifyLinks($reference, $this->datatype->id, $id, $this->request->getLinks($reference), false);
		}

		$this->returnToId($id);
	}

	function __sortTypes($a, $b) {
		return strcoll($a, $b);
	}
}

?>
