<?php
class PXModuleMain extends PXModule {
	private $rootFormatId;
	private $objectsLoadMode;

	function __construct($area, $settings) {
		parent::__construct($area, $settings);
		$this->rootFormatId    = $settings['rootFormat'];

		$this->objectsLoadMode = array(
			PP_CHILDREN_FETCH_NONE     => 'none',
			PP_CHILDREN_FETCH_SELECTED => 'selected',
			PP_CHILDREN_FETCH_ALL      => 'all',
			PP_CHILDREN_FETCH_PAGED    => 'paged'
		);
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
		$this->loadObjects($this->tree, $urlFile, $this->objects);
		$this->objects->getLinks();
		PXProfiler::end();

		// Loading subcontent objects
		PXProfiler::begin('load subObjects');
		$this->loadObjects($this->objects, $urlPart, $this->subObjects);
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
			!($this->request->isIndexFile($urlFile) || $this->objects->hasCurrent())    ||
			!($this->request->isIndexFile($urlPart) || $this->subObjects->hasCurrent())
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

	private function loadObjects($object, $pathName, $fillObj) {
		if (!$object->hasCurrent()) {
			return;
		}

		$allowed = $object->getAllowedChilds();
		
		if (!count($allowed)) {
			return;
		}

		foreach ($allowed as $type => $behaviour) {
			$format = PXRegistry::getTypes($type);

			if(!is_object($format)) {
				continue;
			}

			$loadingMethod  = 'load'.ucfirst($this->objectsLoadMode[$behaviour]).'Objects';
			$objsArray      = $this->$loadingMethod($object, $format, $pathName);

			$fillObj->add($type, $objsArray);
			$fillObj->findCurrent($type, $pathName);
		}
	}
	
	private function loadAllObjects($object, $format, $pathName) {
		$objsArray = array();
		
		$objsArray = $this->_loadContent($format, true, 'parent', $object->currentId);
		
		return $objsArray;
	}

	private function loadNoneObjects($object, $format, $pathName) {
		return array();
	}

	private function loadPagedObjects($object, $format, $pathName) {
		$objsArray = array();

		if (!$this->request->isIndexFile($pathName)) {
			$objsArray = $this->_loadContentLimited($format, true, array('pathname' => $pathName, 'parent' => $object->currentId), 'IGNORED', 1, 0);

		} else {
			$a_per_page  = $this->app->getProperty(strtoupper($format->id).'_PER_PAGE', DEFAULT_CHILDREN_PER_PAGE);
			$count       = $this->_loadContent($format, true, 'parent', $object->currentId, DB_SELECT_COUNT);
			$currentPage = $this->getCurrentPage($format, $count, $a_per_page);

			$this->layout->assign('FP_' . strtoupper($format->id) . '_TOTAL',        $count);
			$this->layout->assign('FP_' . strtoupper($format->id) . '_PER_PAGE',     $a_per_page);
			$this->layout->assign('FP_' . strtoupper($format->id) . '_DEFAULT_PAGE', $currentPage);

			$objsArray = $this->_loadContentLimited($format, true, 'parent', $object->currentId, $a_per_page, $a_per_page*($currentPage-1));
		}

		return $objsArray;
	}
	
	private function loadSelectedObjects($object, $format, $pathName) {
		$objsArray = array();
		
		if (!isset($format->fields['pathname'])) {
			return;
		}

		$objsArray = $this->_loadContentLimited($format, true, array('pathname' => $pathName, 'parent' => $object->currentId), 'IGNORED', 1, 0);
		return $objsArray;
	}
	
	private function getCurrentPage($format, $count, $a_per_page) {
		// default page - from properties.ini (first or last)
		$defaultPage = $this->app->getProperty('FP_' . strtoupper($format->id).'_DEFAULT_LAST_PAGE') ? ceil($count/$a_per_page) : 1;

		$currentPage = $this->request->getVar('page', $defaultPage);
		$currentPage = max(1, ($currentPage > ceil($count/$a_per_page) ? ceil($count/$a_per_page) : $currentPage));

		return $currentPage;
	}

	function adminIndex() {
		if (!isset($this->app->types[$this->rootFormatId])) {
			FatalError("Некорректный тип данных");
		}

		$app     = $this->app;
		$request = $this->request;
		$layout  = $this->layout;

		$rootFormat = $this->rootFormat = $app->types[$this->rootFormatId];

		$rqSid = $request->getSid();
		$rqCid = $request->getCid();

		$hTree = new PXAdminTreeObjects($this->rootFormat);
		$hTree->addToParent('INNER.0.0');
		$hTree->hideCaption();
		$hTree->showChildren('sid');

		if ($rqSid != null && $hTree->has($rqSid)) {
			$hTree->setSelected($rqSid);

			$parentObject = $hTree->get($rqSid);

			$layout->assignTitle('раздел &laquo;'.$parentObject['title'].'&raquo;');


			$cidType   = null;
			$cidObject = null;

			$allowedChilds = $app->getAllowedChildsKeys($this->rootFormat->id, $parentObject);
			if (count($allowedChilds)) {
				foreach ($allowedChilds as $childFormat) {
					if (!isset($app->types[$childFormat])) {
						$layout->append('INNER.1.0', '<h1 class="error">Формат '.$childFormat.' не описан</h1>');
						continue;
					}

					if ($app->types[$childFormat]->struct == 'tree' && $request->GetVar($childFormat.'_view') != 'plain') {
						$objects = new PXAdminTreeObjects($childFormat, $rqSid, 'ByParent');

					} else {
						$objects =  new PXAdminTableObjects($childFormat, $rqSid, 'ByParent');
					}

					$objects->addToParent('INNER.1.0');
					$objects->showChildren('cid');

					if ($rqCid && $childFormat == $request->getCtype()) {
						$layout->setGetVarToSave('ctype', $request->getCtype());

						if ($objects->has($rqCid)) {
							$cidType   = $childFormat;
							$cidObject = $objects->get($rqCid);
						}
					}
				}

				if ($rqCid && !empty($cidType)) {
					$layout->setGetVarToSave('cid', $rqCid);

					$allowedChilds = $app->getAllowedChildsKeys($cidType, $cidObject);
					$layout->setThreeColumns();

					if (count($allowedChilds)) {
						foreach ($allowedChilds as $childFormat) {
							if ($app->types[$childFormat]->struct == 'tree' && $request->GetVar($childFormat.'_view') != 'plain') {
								$objects = new PXAdminTreeObjects($childFormat, $rqCid, 'ByParent');

							} else {
								$subObjects =  new PXAdminTableObjects($childFormat, $rqCid, 'ByParent');
							}

							$subObjects->addToParent('INNER.2.0');
						}

					} else {
						$this->layout->notSetAllowedChilds('INNER.2.0', $cidType, $rqCid);
					}
				}

			} else {
				$this->layout->notSetAllowedChilds('INNER.1.0', $this->rootFormatId, $rqSid);
			}

		} elseif ($rqSid != null) {
			$layout->assign('INNER.1.0', '<H2 class="error">Раздел '.$rqSid.' не найден</H2>');

		} else {
			$layout->assign('INNER.1.0', '<H2>Выберите раздел</H2>');
		}
	}

	function _loadContent(&$format, $status, $param, $value, $flag = DB_SELECT_TABLE, $order = NULL){
		return $this->db->GetObjectsByField($format, $status, $param, $value, $flag, $order);
	}

	function _loadContentLimited(&$format, $status, $param, $value, $limit, $offset, $flag = DB_SELECT_TABLE, $order = NULL){
		return $this->db->GetObjectsByFieldLimited($format, $status, $param, $value, $limit, $offset, $flag, $order);
	}
}
?>