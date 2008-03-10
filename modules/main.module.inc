<?
class PXPage {
	var $sId;
	var $cId;
	var $scId;

	var $sType;
	var $cType;
	var $scType;

	var $rootId;
	var $pathId;

	function PXPage () {
		$this->sId    = -1;
		$this->cId    = -1;
		$this->scId   = -1;

		$this->sType  = -1;
		$this->cType  = -1;
		$this->scType = -1;

		$this->pathId = array();
		$this->rootId = -1;
	}

	function setPathId($pathId) {
		if(!is_array($pathId)) {
			FatalError('pathId должен быть массивом');
		}

		$this->pathId = $pathId;
	}

	function setSId($id) {
		if(!is_numeric($id)) {
			FatalError('sId должен быть числом');
		}

		$this->sId = $id;
	}

	function setRootId($id) {
		if(!is_numeric($id)) {
			FatalError('rootId должен быть числом');
		}

		$this->rootId = $id;
	}

	function setCId($id) {
		if(!is_numeric($id)) {
			FatalError('cId должен быть числом');
		}

		$this->cId = $id;
	}

	function setCType($type) {
		if(!is_string($type)) {
			FatalError('cType должен быть строкой');
		}

		$this->cType = $type;
	}

	function setSType($type) {
		if(!is_string($type)) {
			FatalError('sType должен быть строкой');
		}

		$this->sType = $type;
	}
}

class PXModuleMain extends PXAbstractModule {
	var $rootFormatId;

	function PXModuleMain($area, $settings) {
		$this->PXAbstractModule($area, $settings);
		$this->rootFormatId = $settings['rootFormat'];
	}

	function userIndex($app, $request, $user, $db, $layout, &$tree,
		&$objects, &$subObjects, &$references, &$heap, &$currentSid, &$currentCid,
		&$currentCtype, &$currentSCid, &$currentSCtype, &$rootId, &$pathId,
		&$urlFile, &$error) {

		$page          = new PXPage();

		$this->app     = $app;
		$this->db      = $db;
		$this->user    = $user;
		$this->request = $request;
		$this->page    = $page;

		$urlFile = $this->request->GetFile();
		$urlPart = $this->request->GetPart();

		// Loading struct tree
		NLProfiler("load tree", "BEGIN");
		$this->_loadTree();
		NLProfiler("load tree", "END");

		// Loading objects
		NLProfiler("load objects", "BEGIN");
		$objects = $this->_loadObjects($urlFile);
		NLProfiler("load objects", "END");

		// Loading subobjects
		NLProfiler("load subobjects", "BEGIN");
		$subObjects = $this->_loadSubObjects($urlPart, $objects);
		NLProfiler("load subobjects", "END");

		// return vars by references
		$currentSid    = $page->sId;
		$currentCid    = $page->cId;
		$currentSCid   = $page->scId;

		$currentCtype  = $page->cType;
		$currentSCtype = $page->scType;

		$pathId        = $page->pathId;
		$rootId        = $page->rootId;
		$tree	       = $this->tree;

		// Loading objects references
		NLProfiler("Loading references", "BEGIN");
		$this->_loadReferences($this->rootFormatId, $currentSid,  $references, $heap);
		$this->_loadReferences($this->page->cType,  $currentCid,  $references, $heap);
		$this->_loadReferences($this->page->scType, $currentSCid, $references, $heap);
		$this->_loadReferences('suser',             $user->id,    $references, $heap);
		NLProfiler("Loading references", "END");

		// Определение кода ошибки
		switch(true) {
			case ($currentSid == -1):
			case ($urlFile != 'index.html' && $currentCid  == -1):
			case ($urlPart != ''           && $currentSCid == -1):
				$error = 404;
				break;

			default:
				$error = 0;
				break;
		}
	}

	function _initAliases() {
		if(!isset($this->settings['domainAlias'])) {
			$this->settings['domainAlias'] = array();
		}

		$aliases = $this->settings['domainAlias'];

		if(is_string($aliases)) {
			$aliases = array($aliases);
		}

		return $aliases;
	}

	function _findDomainAlias() {
		$host =  $this->request->getHttpHost();

		// находим соответствие между хостом, на который мы зашли
		// и корнем в дереве, который может быть нам нужен
		$hostAlias = 'default';
		foreach($this->_initAliases() as $tmp) {
			$alias = preg_split('/\s*=\s*/', trim($tmp));

			if($alias[0] == $host) {
				$hostAlias = $alias[1];
				break;
			}
		}

		// если алиас "не нашли", может быть нужный хост есть среди корней?
		if($hostAlias == 'default') {
			foreach($this->tree->levels[1] as $_rootId) {
				if($this->tree->leafs[$_rootId]->content['pathname'] === $host) {
					$hostAlias = $host;
					break;
				}
			}
		}

		// находим нужный корень и правим его pathname
		foreach($this->tree->levels[1] as $_rootId) {
			if($this->tree->leafs[$_rootId]->content['pathname'] === $hostAlias) {
				$this->tree->leafs[$_rootId]->content['pathname'] = $host;
				break;
			}
		}
	}

	function _findTreeContext() {
		// "пустой" проект
		if(!isset($this->tree->levels[1])) {
			return;
		}

		$tree = $this->tree;

		// Guessing context
		$this->_findDomainAlias();
		$urlPath = $this->request->getHostAndDir();

		$pathId  = $tree->getIdArrayByPath('pathname', $urlPath);

		$this->page->setPathId($pathId);
		$this->page->setSId(end($pathId));
		$this->page->setRootId(reset($pathId));
	}

	function _loadTree() {
		$tree = $this->db->getObjects($this->db->types[$this->rootFormatId], true, DB_SELECT_TREE);

		// Dirty hack!!!
		// Fix tree references
		foreach ($tree->leafs as $k => $l) {
			$tree->leafs[$k]->tree = $tree;
		}

		$this->tree = $tree;
		$this->_findTreeContext();
	}

	function _loadChilds($allowed, $childs, &$currentId, &$currentType, $parent, $pathName) {
		if (!count($allowed)) {
			return array();
		}

		foreach ($allowed as $type=>$behaviour) {
			if(!isset($this->app->types[$type])) {
				continue;
			}

			$format = $this->app->types[$type];

			switch ($behaviour) {
				case PP_CHILDREN_FETCH_ALL:
					$childs[$type] = $this->db->getObjectsByParent($format, true, $parent);

					foreach ($childs[$type] as $o) {
						if (isset($format->fields['pathname']) and $o['pathname'] == $pathName) {
							$currentId  = $o['id'];
							$currentType = $type;
							break;
						}
					}
					break;

				case PP_CHILDREN_FETCH_SELECTED:
					if (isset($format->fields['pathname'])) {
						$tmp = $this->db->getObjectsByFieldLimited($format, true, array('pathname' => $pathName, 'parent' => $parent), 'IGNORED', 1, 0);

						if (count($tmp)) {
							$childs[$type] = $tmp;

							$tmpId = current(array_keys($childs[$type]));

							$currentId   = $tmpId;
							$currentType = $type;
						}
					}
					break;

				case PP_CHILDREN_FETCH_NONE:
					break;
			}
		}

		return $childs;
	}

	function _loadObjects($pathname) {
		if(!isset($this->tree->leafs[$this->page->sId])) {
			return;
		}

		//check me!

		$current = $this->tree->leafs[$this->page->sId];
		$allowed = $this->app->getAllowedChilds($this->rootFormatId, $current);
		$objects = array();

		return $this->_loadChilds($allowed, $objects, $this->page->cId, $this->page->cType, $this->page->sId, $pathname);
	}

	function _loadSubObjects($pathname, $objects) {
		$currentCtype = $this->page->cType;
		$currentCid   = $this->page->cId;

		if(!isset($objects[$currentCtype][$currentCid])) {
			return;
		}

		$current =& $objects[$currentCtype][$currentCid];

		$allowed    =  $this->app->getAllowedChilds($this->page->cType, $current);
		$subObjects = array();

		return $this->_loadChilds($allowed, $subObjects, $this->page->scId, $this->page->scType, $this->page->cId, $pathname);
	}

	function _loadReferences($type, $id, &$references, &$heap) {
		$app = $this->app;
		$db  = $this->db;

		if(!is_numeric($id) || $id === -1) {
			return;
		}

		if(!isset($app->types[$type]->references)) {
			return;
		}

		foreach ($app->types[$type]->references as $ref) {
			if($ref->load == 'manual') {
				continue;
			}

			$references[$ref->name] = $db->getLinks($ref, $type, $id);
			$heap[$ref->getOther($type)] = $db->getObjectsByIdArray($app->types[$ref->getOther($type)], true, array_keys($references[$ref->name]));
		}
	}

	function adminIndex(&$app, &$request, &$user, &$db, &$layout) {
		if (!isset($app->types[$this->rootFormatId])) {
			FatalError("Некорректный тип данных");
		}
		$rootFormat = &$app->types[$this->rootFormatId];
		$tree       = $db->GetObjects($rootFormat, null, DB_SELECT_TREE);
		$rqSid      = $request->GetSid();

		if ($rqSid != null && isset($tree->leafs[$rqSid])) {
			$parentObjectPath = $tree->getFullPathString($rqSid);
		}

		$db->filterByAccess('admin', $rootFormat, $tree);
		$layout->AssignTree('INNER.0.0', $rootFormat, $tree, $request->GetSid(), $request->GetLeafStatus());
		$layout->AssignControls('INNER.0.1', $request->GetSid(), array($this->rootFormatId));
		$layout->AssignContextMenu('INNER.0.0.CONTEXT', 0, array($this->rootFormatId));

		if ($rqSid != null && isset($tree->leafs[$rqSid])) {
			$parentObject = $db->GetObjectByID($rootFormat, $rqSid);

			$layout->AssignTitle('раздел &laquo;'.$parentObject['title'].'&raquo;');

			$allowedChilds = $app->GetAllowedChildsKeys($rootFormat->id, $parentObject);
			$layout->AssignControls('INNER.1.1', $rqSid, $allowedChilds);
			$layout->AssignContextMenu('INNER.1.0.CONTEXT', $rqSid, $allowedChilds);

			$cidType   = null;
			$cidObject = null;
			$onPage    = $app->GetProperty('CHILDREN_ON_PAGE', 20);
			if (count($allowedChilds)) {
				foreach ($allowedChilds as $childFormat) {
					$layout->SetGetVarToSave($childFormat.'_page', $request->GetVar($childFormat.'_page', 1));
					$layout->SetGetVarToSave($childFormat.'_view', $request->GetVar($childFormat.'_view', 'plain'));
				}

				foreach ($allowedChilds as $childFormat) {
					if (!isset($app->types[$childFormat])) {
						$layout->append('INNER.1.0', '<h1 class="error">Формат '.$childFormat.' не описан</h1>');
						continue;
					}

					if ($app->types[$childFormat]->struct == 'tree' && $request->GetVar($childFormat.'_view') != 'plain') {
						$objects = $db->GetObjectsByParent($app->types[$childFormat], null, $rqSid, DB_SELECT_TREE);
						$layout->AppendTableAsTree('INNER.1.0', $childFormat, $objects, $rqSid, $request->GetLeafStatus());

					} else {
						$count   = $db->GetObjectsByParent($app->types[$childFormat], null, $rqSid, DB_SELECT_COUNT);
						$currentPage  = $request->GetVar($childFormat.'_page', 1);
						$currentOrder = $request->GetVar($childFormat.'_order', $app->types[$childFormat]->order);
						$layout->SetGetVarToSave($childFormat.'_order', $currentOrder);

						$currentPage = $currentPage > ceil($count/$onPage) ? ceil($count/$onPage) : $currentPage;

						$objects = $db->GetObjectsByParentLimited($app->types[$childFormat], null, $rqSid, $onPage, $onPage*($currentPage-1), DB_SELECT_TABLE, $currentOrder);

						$table = new PXAdminTable($objects, $app->types[$childFormat], $layout->getData);
						$table->loadDropdownValues($db);
						$table->setPager($currentPage, $onPage, $count);
						$table->setSelected($request->GetCid());
						$table->setParentPathName($parentObjectPath);
						$table->setShowChildObjects(true);
						$layout->append('INNER.1.0', $table->getTable());
					}

					if ($request->GetCid()) {
						$pos = array_search($request->GetCid(), GetColFromTable($objects, 'id')); // FIXME

						if (isset($objects[$request->GetCid()])) {
							$cidType   = $childFormat;
							$cidObject = $objects[$request->GetCid()];
						}
					}
				}

				if ($request->GetCid() && !empty($cidType)) {
					$layout->SetGetVarToSave('cid', $request->GetCid());
					$allowedChilds = $app->GetAllowedChildsKeys($cidType, $cidObject);
					$layout->SetSimpleInnerLayout(array('25%', '40%', '35%'), array('100%', ''));

					if (count($allowedChilds)) {
						foreach ($allowedChilds as $childFormat) {
							$layout->SetGetVarToSave($childFormat.'_page', $request->GetVar($childFormat.'_page', 1));
							$layout->SetGetVarToSave($childFormat.'_view', $request->GetVar($childFormat.'_view', 'plain'));
						}

						foreach ($allowedChilds as $childFormat) {
							if ($app->types[$childFormat]->struct == 'tree' && $request->GetVar($childFormat.'_view') != 'plain') {
								$objects = $db->GetObjectsByParent($app->types[$childFormat], null, $request->GetCid(), DB_SELECT_TREE);
								$layout->AppendTableAsTree('INNER.2.0', $childFormat, $objects, $request->GetCid(), $request->GetLeafStatus());

							} else {

								$count   = $db->GetObjectsByParent($app->types[$childFormat], null, $request->GetCid(), DB_SELECT_COUNT);
								$currentPage  = $request->GetVar($childFormat.'_page', 1);
								$currentOrder = $request->GetVar($childFormat.'_order', $app->types[$childFormat]->order);
								$layout->SetGetVarToSave($childFormat.'_order', $currentOrder);

								$currentPage = $currentPage > ceil($count/$onPage) ? ceil($count/$onPage) : $currentPage;

								$objects = $db->GetObjectsByParentLimited($app->types[$childFormat], null, $request->GetCid(), $onPage, $onPage*($currentPage-1), DB_SELECT_TABLE, $currentOrder);

								$table = new PXAdminTable($objects, $app->types[$childFormat], $layout->getData);
								$table->loadDropdownValues($db);
								$table->setPager($currentPage, $onPage, $count);
								if (isset($cidObject['pathname'])) {
									$table->setParentPathName($parentObjectPath.$cidObject['pathname'].'?part=');
								}
								$layout->append('INNER.2.0', $table->getTable());
							}
						}

						$layout->AssignContextMenu('INNER.2.0.CONTEXT', $request->GetCid(), $allowedChilds);
						$layout->AssignControls('INNER.2.1', $request->GetCid(), $allowedChilds);

					} else {
						$layout->Assign('INNER.2.0', '
						<H2>Для добавления в раздел информации необходимо указать разрешенные форматы</H2>
						<UL><LI><A href="javascript:EditContent(\''.$cidType.'\', \''.$request->GetCid().'\', \'children\')">Разрешенные форматы раздела</A></LI></UL>
						');
					}
				}
			} else {
				$layout->Assign('INNER.1.0', '
				<H2>Для добавления в раздел информации необходимо указать разрешенные форматы</H2>
				<UL><LI><A href="javascript:EditContent(\''.$this->rootFormatId.'\', \''.$rqSid.'\', \'children\')">Разрешенные форматы раздела</A></LI></UL>
				');
			}
		} elseif ($rqSid != null) {
			$layout->Assign('INNER.1.0', '<H2 class="error">Раздел '.$rqSid.' не найден</H2>');
		} else {
			$layout->Assign('INNER.1.0', '<H2>Выберите раздел</H2>');
		}
	}

}

?>
