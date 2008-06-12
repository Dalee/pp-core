<?php
class PXModuleMain extends PXAbstractModule {
	private $rootFormatId;

	function __construct($area, $settings) {
		parent::__construct($area, $settings);
		$this->rootFormatId = $settings['rootFormat'];
	}

	function userIndex() {
		$urlFile = $this->request->GetFile();
		$urlPart = $this->request->GetPart();

		// Loading struct tree
		NLProfiler('load struct', 'BEGIN');
		$this->_loadTree();
		NLProfiler('load struct', 'END');

		$this->struct->load();

		// Loading content objects
		NLProfiler('load content', 'BEGIN');
		$this->struct->loadChilds($this->content, $urlFile);
		NLProfiler('load content', 'END');


		// Loading subcontent objects
		NLProfiler('load subcontent', 'BEGIN');
		$this->content->loadChilds($this->subContent, $urlPart);
		NLProfiler('load subcontent', 'END');


		// Loading objects references
		NLProfiler('Loading references', 'BEGIN');
		$this->struct->getLinks();
		$this->content->getLinks();
		$this->subContent->getLinks();
		// $this->user->getLinks(); -- what we must do ???
		NLProfiler('Loading references', 'END');

		$this->check404error($urlFile, $urlPart);
	}

	function check404error($urlFile, $urlPart) {
		// Определение кода ошибки
		switch(true) {
			case (!$this->struct->hasCurrent()):
			case ($urlFile != 'index.html' && !$this->content->hasCurrent()):
			case ($urlPart != ''           && !$this->subContent->hasCurrent()):
				$error = 404;
				break;

			default:
				$error = 0;
				break;
		}
		PXRegistry::setVar('error', $error);
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

	function _loadTree() {
		$this->struct->format = $this->app->types[$this->rootFormatId];
		$this->struct->load();
		$this->_findTreeContext();
	}

	function _findTreeContext() {
		// "пустой" проект
		if(!isset($this->struct->tree->levels[1])) {
			return;
		}

		// Guessing context
		$this->_findDomainAlias();
		$urlPath = $this->request->getHostAndDir();
		$this->struct->findPath($urlPath);
	}

	function _findDomainAlias() {
		$host =  $this->request->getHttpHost();
		$tree = $this->struct->tree;

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
			foreach($tree->levels[1] as $_rootId) {
				if($tree->leafs[$_rootId]->content['pathname'] === $host) {
					$hostAlias = $host;
					break;
				}
			}
		}

		// находим нужный корень и правим его pathname
		foreach($tree->levels[1] as $_rootId) {
			if($tree->leafs[$_rootId]->content['pathname'] === $hostAlias) {
				$tree->leafs[$_rootId]->content['pathname'] = $host;
				break;
			}
		}
	}

	function adminIndex() {
		if (!isset($this->app->types[$this->rootFormatId])) {
			FatalError("Некорректный тип данных");
		}

		$app     =& $this->app;
		$db      =& $this->db;
		$request =& $this->request;
		$layout  =& $this->layout;

		$rootFormat = &$app->types[$this->rootFormatId];
		$tree       = $db->getObjects($rootFormat, null, DB_SELECT_TREE);
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
