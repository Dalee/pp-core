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
		$currentOrder = $this->request->GetOrderVar($format, $this->app->types[$format]->order, $this->app->types[$format]->fields);
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

		$action = $request->getAction();

		$layout->setGetVarToSave('id',     $request->getId());
		$layout->setGetVarToSave('format', $this->datatype->id);
		$layout->setGetVarToSave('action', $action);

		$layout->setOuterForm('action.phtml', 'POST', 'multipart/form-data');

		$this->object = $this->popupGetObject();
		$this->form = new PXAdminObjectForm($this->datatype, $this->object);

		$this->popupSetMenu();
		$this->popupCheckAccess();

		$layout->assign('OUTER.LEFTCONTROLS',  $this->form->leftControls());
		$layout->assign('OUTER.RIGHTCONTROLS', $this->form->rightControls());
		$db->LoadDirectoriesByType($this->datatype, $this->object);

		switch ($action) {
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

			case 'auditlog':
				$this->popupAuditLog();
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

			if(!$this->user->can('admin', $this->datatype, $object)) {
				$object = array();
				$rId = 0;
			}
		}

		if(isset($object['sys_owner'])) {
			$tmp = $this->db->getObjectById($this->app->types['suser'], $object['sys_owner']);
			$object['ownerlogin'] = isset($tmp['title']) ? $tmp['title'] : '';
		}

		return $object;
	}

	// protected
	function popupGetMenuItems() {
		$menuItems = array();
		$menuItems['main'] = 'Основная информация';

		if (isset($this->object['id']) && $this->object['id'] != 0) {
			if (count($this->datatype->childs)) {
				$menuItems['children'] = 'Разрешенные дети';
			}

			if (!empty($this->datatype->references)) {
				$menuItems['links'] = 'Связи';
			}

			$menuItems['auditlog'] = 'Журнал';
			$menuItems['remove']   = 'Удаление';
		}

		return $menuItems;
	}

	function popupSetMenu() {
		$this->layout->setMenu($this->popupGetMenuItems(), $this->request->getAction(), 'action');
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
		$this->layout->append('OUTER.CONTENT', $this->form->MainForm($this->datatype));
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

	function popupAuditLog() {
		$filters = array(
			'format' => $this->datatype->id,
			'id'     => $this->object['id']
		);

		require_once 'libpp/modules/auditlog.module.inc';
		$this->viewer = new PXAuditViewer($filters);

		$page    = $this->request->getVar('_page', 1);
		$onPage  = $this->app->getProperty('CHILDREN_ON_PAGE', 15);

		$count = $this->viewer->getCount();
		$data  = $this->viewer->getData($page, $onPage);

		if($count) {
			$caption  = '<a href="./?area=auditlog&format='.$this->datatype->id.'&id='.$this->object['id'].'" target="_blank">Журнал аудита</a>, ';
			$caption .= $count.' '.NumericEndingsRussian($count, 'запись', 'записи', 'записей');
			$caption .= ' для текущего объекта';

			$table = new PXAdminAuditTable();
			$table->setDict('level', $this->viewer->getErrorLevels());
			$table->setData($data);
			$table->setCaption($caption);

			$this->layout->append('OUTER.CONTENT', $table->html());

			if($count > $onPage) {
				$param = $this->request->getAllGetData();

				$pager = new PxAdminPager($page, $onPage, $count, '',  $param);
				$this->layout->append('OUTER.CONTENT', $pager->html());
			}
		}

	}

	function popupLinks() {
		$db       =& $this->db;
		$app      =& $this->app;
		$datatype =& $this->datatype;
		$request  =& $this->request;
		$layout   =& $this->layout;
		$object   =& $this->object;
		
		$refFilters    = (array)$request->GetVar('filters', array());

		foreach ($datatype->references as $k=>$reference) {
			if($reference->hidden){
				continue;
			}
			$refDatatype   = $app->types[$k];
			
			$existingLinks = $db->getLinks($reference, $datatype->id, $object['id']);

			$onPage        = $app->getProperty('LINKS_ON_PAGE', 10);
			$currentPage   = $request->getVar($k.'_page', 1);
			$layout->setGetVarToSave($k.'_page', $currentPage);

			$onlyExistingLinks = $request->GetVar($k.'_exist', $reference->byDefault);
			$layout->setGetVarToSave($k.'_exist', $onlyExistingLinks);

			$filteredWhere = $this->applyFiltersToWhere($refFilters, $refDatatype);
			$trueCond      = $db->TrueStatusString() . '=' . $db->TrueStatusString();
			$falseCond     = $db->TrueStatusString() . '=' . $db->TrueStatusString(false);
			
			if($onlyExistingLinks) {
				if(count($existingLinks)){
					$existingLinksWhere = $k . '.id IN ( ' .join(',',array_keys($existingLinks)). ' ) ' ;
				} else {
					$existingLinksWhere = $falseCond;
				}
				list($possibleLinks, $pagerCount, $overallCount) = $this->_getLinksData($refDatatype, $existingLinksWhere . $filteredWhere, $trueCond, $onPage, $currentPage);
			} else {
				$mainWhere = $trueCond; //No stupid NULL

				if(!empty($reference->filterFrom) && $reference->from == $k) {
					$mainWhere = '(' . $db->parseWhereTemplate($reference->filterFrom, $object, $datatype) . ')';
				}

				if(!empty($reference->filterTo) && $reference->to == $k) {
					$mainWhere = '(' . $db->parseWhereTemplate($reference->filterTo, $object, $datatype) . ')';
				}

				if (!empty($reference->restrictBy)) {
					$mainWhere .= ' AND ';
					$mainWhere .= $k . '.' . $reference->restrictBy." = '".$db->escapeString($object[$reference->restrictBy])."'";
					$mainWhere  = '(' . $mainWhere . ')';
				}
				list($possibleLinks, $pagerCount, $overallCount) = $this->_getLinksData($refDatatype, $mainWhere . $filteredWhere, $mainWhere, $onPage, $currentPage);
				if ($datatype->id == $k) {
					unset($possibleLinks[$object['id']]);
				}
			}
			$layout->append('OUTER.CONTENT', $this->form->linksForm($reference, $refDatatype, $existingLinks, $possibleLinks, $onPage, $currentPage, $pagerCount, $overallCount, $layout, $onlyExistingLinks, $refFilters));
		}
	}
	
	function _getLinksData($dType, $where, $whereAllCount, $onPage, $cPage){
		$pLinks = $this->db->getObjectsByWhereLimited($dType, NULL, $where, $onPage, $onPage*($cPage-1));
		$pCount = $this->db->getObjectsByWhere($dType, NULL, $where, DB_SELECT_COUNT);
		$oCount = $this->db->getObjectsByWhere($dType, NULL, $whereAllCount,  DB_SELECT_COUNT);
		
		return array($pLinks, $pCount, $oCount);
	}

	function adminAction() {
		$this->datatype =& $this->getDatatype();

		$rAction = $this->request->getAction();

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

			case 'clone':
				$this->actionCloneContentObject();
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

	function actionCloneContentObject() {
		$donor = $this->request->getContentObject($this->datatype);

		if(!is_numeric($donor['id']) || !$donor['id']) {
			FatalError('Клонировать можно только существующие объекты');
		}

		$cloneId = $this->db->cloneContentObject($this->datatype, $donor['id']);
		$this->returnToId($cloneId, 'main');
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
	
	function applyFiltersToUri($filters){
		foreach($filters as $dt => $fields){
			if(is_array($fields) && count($fields) > 0){
				foreach($fields as $field => $filter){
					if(empty($filter)) continue;
					$this->nextLocation .= '&filters['.$dt.']'.'['.$field.']='.rawurlencode($filter);
				}
			}
		}
	}
	
	function applyFiltersToWhere($refFilters, $refDatatype){
		$filterOnWhere = NULL;
		if(isset($refFilters[$refDatatype->id])    &&
		   is_array($refFilters[$refDatatype->id]) && 
		   count($refFilters[$refDatatype->id]) > 0
		  ){
			$applied = 0;
			$filterOnWhere = ' AND (';
			foreach($refFilters[$refDatatype->id] as $field => $filter){
				if(empty($filter)) continue;
				$applied++;
				$this->layout->setGetVarToSave('filters['.$refDatatype->id.']['.$field.']', $filter);
				//apply simple filter logic, ex. : ^search string$
				$modifiers = P_LEFT|P_RIGHT;
				if(substr($filter, 0, 1) == '^'){
					$modifiers &= P_RIGHT;
					$filter = substr($filter, 1);
				}
				if(substr($filter, -1) == '$'){
					$modifiers &= P_LEFT;
					$filter  = substr($filter, 0, -1);
				}
				
				// build where 
				$filterOnWhere .= $refDatatype->id . '.' .$this->db->escapeString($field).$this->db->LIKE($filter, $modifiers)." AND ";
			}
			
			if(!$applied){
				$filterOnWhere = NULL;
			} else {
				$filterOnWhere  = substr($filterOnWhere, 0, -4); //remove last AND or OR
				$filterOnWhere .= ')';
				$this->layout->SetGetVarToSave('charcheck', $this->request->GetVar('charcheck'));
			}
		}
		return $filterOnWhere;
	}
	
	function appendCharCheck(){
		$this->nextLocation .= '&charcheck='.urlencode('йцукен');
	}
	
	function appendLinksExistFlag(){
		foreach($this->request->GetAllPostData() as $k => $v){
			if(is_string($k) && strstr($k, '_exist') == '_exist'){
				$this->nextLocation .= "&{$k}={$v}";
			}
		}
	}
	
	function returnToId($id, $action = null) {
		$this->nextLocation .= '&id='.$id;

		if(is_string($action)) {
			$this->nextLocation .= '&action='.$action;
		}
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
			if($reference->hidden){
				continue;
			}
			$ll = $this->_makeLinkedListFrom($reference);
			$this->db->ModifyLinks($reference, $this->datatype->id, $id, $ll, false);
		}
		
		if(count($filters = (array)$this->request->GetVar('filters', array()))){
			$this->applyFiltersToUri($filters);
			$this->appendCharCheck();
		}
		$this->appendLinksExistFlag();
		$this->returnToId($id);
	}
	
	function _makeLinkedListFrom($ref){
		$linkedList = array();
		foreach($this->request->getLinks($ref) as $values){
			foreach($values as $id => $data){
				$node = & $linkedList[$id];
				if(isset($linkedList[$id])){
					while(!is_null($node = & $node['next']));
				}
				$node         = $data;
				$node['next'] = NULL;
			}
		}
		return $linkedList;
	}

	function __sortTypes($a, $b) {
		return strcoll($a, $b);
	}
}

?>
