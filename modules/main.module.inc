<?php
class PXModuleMain extends PXModule {
	private $rootFormatId;

	function __construct($area, $settings) {
		parent::__construct($area, $settings);
		$this->rootFormatId = $settings['rootFormat'];
	}

	function userIndex() {
		// get path
		$urlPath = $this->request->getHostAndDir();
		$urlFile = $this->request->getFile();
		$urlPart = $this->request->getPart();

		$this->tree->setFormat($this->rootFormatId);
		$this->tree->setAliases($this->_parseAliasesConfig());

		// Loading struct tree
		PXProfiler::begin('load tree');
		$this->tree->load($urlPath);

		$this->tree->getLinks();
		PXProfiler::end();

		// Loading content objects
		PXProfiler::begin('load objects');
		$this->tree->loadChilds($this->objects, $urlFile);
		$this->objects->getLinks();
		PXProfiler::end();

		// Loading subcontent objects
		PXProfiler::begin('load subObjects');
		$this->objects->loadChilds($this->subObjects, $urlPart);
		$this->subObjects->getLinks();
		PXProfiler::end();

		// Loading objects references
		PXProfiler::begin('Loading references');
		// $this->user->getLinks(); -- what we must do ???
		PXProfiler::end();

		$this->check404error($urlFile, $urlPart);
	}

	function check404error($urlFile, $urlPart) {
		$response = PXResponse::getInstance();
		$error = 0;

		// Определение кода ошибки
		if (
			!$this->tree->hasCurrent() ||
			($urlFile != 'index.html' && !$this->objects->hasCurrent())    ||
			($urlPart != 'index.html' && !$this->subObjects->hasCurrent())
		) {
			$response->notFound();
		} else {
			$response->setOk();
		}
	}

	function _parseAliasesConfig() {
		if(!isset($this->settings['domainAlias'])) {
			$this->settings['domainAlias'] = array();
		}

		$config = $this->settings['domainAlias'];

		if(is_string($config)) {
			$config = array($config);
		}

		$aliases = array();
		foreach($config as $k=>$tmp) {
			list($host, $alias) = preg_split('/\s*=\s*/', trim($tmp));
			$aliases[$host] = $alias; 
		}

		return $aliases;
	}

	function _loadAdminTree() {
		return $this->db->GetObjects($this->rootFormat, null, DB_SELECT_TREE);
	}

	function adminIndex() {
		if (!isset($this->app->types[$this->rootFormatId])) {
			FatalError("Некорректный тип данных");
		}

		$app     = $this->app;
		$db      = $this->db;
		$request = $this->request;
		$layout  = $this->layout;

		$rootFormat = $this->rootFormat = $app->types[$this->rootFormatId];
		$this->tree = $this->_loadAdminTree();
		$rqSid      = $request->GetSid();

		if ($rqSid != null && isset($this->tree->leafs[$rqSid])) {
			$parentObjectPath = $this->tree->getFullPathString($rqSid);
		}

		$db->filterByAccess('admin', $this->rootFormat, $this->tree);
		$this->rootFormat->makeTreeWithSid = true;
		$this->_assignAdminTree();
		unset($this->rootFormat->makeTreeWithSid);

		if ($rqSid != null && isset($this->tree->leafs[$rqSid])) {
			$parentObject = $db->GetObjectByID($rootFormat, $rqSid);

			$layout->AssignTitle('раздел &laquo;'.$parentObject['title'].'&raquo;');

			$allowedChilds = $app->getAllowedChildsKeys($this->rootFormat->id, $parentObject);

			$layout->AssignControls('INNER.1.1', $rqSid, $allowedChilds);
			$layout->AssignContextMenu('INNER.1.0.CONTEXT', $rqSid, $allowedChilds);

			$cidType   = null;
			$cidObject = null;
			$onPage    = max(1, $app->GetProperty('CHILDREN_ON_PAGE', 20));
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
						$objects = $this->_loadContent($app->types[$childFormat], NULL, 'parent', $rqSid, DB_SELECT_TREE);
						$layout->AppendTableAsTree('INNER.1.0', $childFormat, $objects, $rqSid, $request->GetLeafStatus());

					} else {
						$count   = $this->_loadContent($app->types[$childFormat], NULL, 'parent', $rqSid, DB_SELECT_COUNT);
						$currentPage  = $request->GetVar($childFormat.'_page', 1);
						$currentOrder = $request->GetOrderVar($childFormat, $app->types[$childFormat]->order, $app->types[$childFormat]->fields);
						
						$layout->SetGetVarToSave($childFormat.'_order', $currentOrder);

						$currentPage = max(1, ($currentPage > ceil($count/$onPage) ? ceil($count/$onPage) : $currentPage));

						$objects = $this->_loadContentLimited($app->types[$childFormat], NULL, 'parent', $rqSid, $onPage, $onPage*($currentPage-1), DB_SELECT_TABLE, $currentOrder);

						$table = new PXAdminTable($objects, $app->types[$childFormat], $layout->getData);
						$table->loadDropdownValues($db);
						$table->setPager($currentPage, $onPage, $count);
						$table->setSelected($request->GetCid(), $request->getCtype());
						$table->setParentPathName($parentObjectPath);
						$table->setShowChildObjects(true);
						$table->setChildLink('cid');

						$layout->append('INNER.1.0', $table->getTable());
					}

					if ($request->GetCid() && $childFormat == $request->getCtype()) {
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
					$layout->setThreeColumns();

					if (count($allowedChilds)) {
						foreach ($allowedChilds as $childFormat) {
							$layout->SetGetVarToSave($childFormat.'_page', $request->GetVar($childFormat.'_page', 1));
							$layout->SetGetVarToSave($childFormat.'_view', $request->GetVar($childFormat.'_view', 'plain'));
						}
						
						foreach ($allowedChilds as $childFormat) {
							if ($app->types[$childFormat]->struct == 'tree' && $request->GetVar($childFormat.'_view') != 'plain') {
								$objects = $this->_loadContent($app->types[$childFormat], NULL, 'parent', $request->GetCid(), DB_SELECT_TREE);
								$layout->AppendTableAsTree('INNER.2.0', $childFormat, $objects, $request->GetCid(), $request->GetLeafStatus());
							} else {
								$count   = $this->_loadContent($app->types[$childFormat], NULL, 'parent', $request->GetCid(), DB_SELECT_COUNT);
								$currentPage  = $request->GetVar($childFormat.'_page', 1);
								$currentOrder = $request->GetOrderVar($childFormat, $app->types[$childFormat]->order, $app->types[$childFormat]->fields);
								
								$layout->SetGetVarToSave($childFormat.'_order', $currentOrder);

								$currentPage = max(1, ($currentPage > ceil($count/$onPage) ? ceil($count/$onPage) : $currentPage));

								$objects = $this->_loadContentLimited($app->types[$childFormat], NULL, 'parent', $request->GetCid(), $onPage, $onPage*($currentPage-1), DB_SELECT_TABLE, $currentOrder);

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

	function _assignAdminTree(){
		$this->layout->AssignTree('INNER.0.0', $this->rootFormat, $this->tree, $this->request->GetSid(), $this->request->GetLeafStatus());
		$this->layout->AssignControls('INNER.0.1', $this->request->GetSid(), array($this->rootFormatId));
		$this->layout->AssignContextMenu('INNER.0.0.CONTEXT', 0, array($this->rootFormatId));
	}

	function _loadContent(&$format, $status, $param, $value, $flag = DB_SELECT_TABLE, $order = NULL){
		return $this->db->GetObjectsByField($format, $status, $param, $value, $flag, $order);
	}

	function _loadContentLimited(&$format, $status, $param, $value, $limit, $offset, $flag = DB_SELECT_TABLE, $order = NULL){
		return $this->db->GetObjectsByFieldLimited($format, $status, $param, $value, $limit, $offset, $flag, $order);
	}
}
?>
