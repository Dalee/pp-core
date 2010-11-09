<?php

require_once BASEPATH . "libpp/modules/file.module.inc";

class PXModuleRegionalFile extends PXModuleFile {
	function adminIndex() {
		$this->_addRegionSuffix();
		parent::adminIndex();
	}
	
	function adminAction() {
		$this->_addRegionSuffix();
		return parent::adminAction();
	}
	
	function adminPopup() {
		$this->_addRegionSuffix();
		return parent::adminPopup();
	}

	protected
	function _addRegionSuffix(){
		$regions        = PXMultiRegions::getInstance();

		$isTree         = $regions->isTree();
		$myRegions      = $regions->getRegionsAsDirectory(false);
		$allRegionsData = $regions->getRegionsData($isTree);
		$rid            = $regions->getRid();

		if ($isTree && is_object($myRegions)) { //work with tree
			$myRegions = $rid && isset($myRegions->leafs[$rid]) 
				? array($myRegions->leafs[$rid]->id) : (isset($myRegions->levels[1]) ? $myRegions->levels[1] : array()); //show only first level pathnames because other levels already inside
		} else { //work with list
			$myRegions = $rid && isset($myRegions[$rid]) ? array($rid) : array_keys($myRegions);
		}
		
		$fieldname = 'shortname';
		$params = PXRegistry::getApp()->plugins['multipleregions']->params;
		if(array_key_exists('fieldname', $params)) {
			$fieldname = $params['fieldname'];
		}

		if (sizeof($this->settings) && sizeof($myRegions)) {			
			$paths          = array_flip($this->settings);
			$this->settings = array();

			foreach ($myRegions as $regionId) {
				$suffix = $this->{$isTree ? 'createDirectoryTree' : 'createSingleDirectory'}($allRegionsData, $regionId, $fieldname);
				$currentPaths = $paths;
				array_walk($currentPaths, array($this, '_addSuffix'), $suffix);
				$this->settings = array_merge($this->settings, array_flip($currentPaths));
			}
		}
	}

	protected
	function _addSuffix(&$url, $_K, $suffix){
		$url = $url . $suffix;
	}

	protected
	function createDirectoryTree($tree, $id, $fieldname = 'shortname', $delim = '/~') {
		if (isset($tree->leafs[$id]) && isset($tree->leafs[$id]->content[$fieldname])) {
			$path = $tree->leafs[$id]->content[$fieldname];
		} else {
			$path = '';
		}

		if (isset($tree->leafs[$id]->parent) && $tree->leafs[$id]->parent) {
			return $this->createDirectoryTree($tree, $tree->leafs[$id]->parent, $fieldname) . $delim . $path;
		}
		
		return $delim . $path;
	}

	protected
	function createSingleDirectory($allRegionsData, $regionId, $fieldname = 'shortname', $delim = '/~'){
		return isset($allRegionsData[$regionId]) && isset($allRegionsData[$regionId][$fieldname]) ? $delim . $allRegionsData[$regionId][$fieldname] : '';
	}
}
?>
