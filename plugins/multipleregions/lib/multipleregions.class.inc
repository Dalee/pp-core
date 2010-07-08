<?php
class PXMultiRegions {
	private $db;
	private $request;
	private $rid;
	private $context;
	private static $self;

	const REGION_MARK = 'sys_regions';

	private function __construct(){
		$this->db      = PXRegistry::getDb();
		$this->request = PXRegistry::getRequest();
	}
	
	function getInstance($context = NULL){ //context: ENUM {"user", "admin"}
		if(empty(self::$self)){
			$klass = __CLASS__;
			self::$self = new $klass();
		}
		if($context){
			self::$self->context = $context;
		}
		return self::$self;
	}

	function getRegionsData(){
		static $regions_data;
		if(!isset($regions_data)){
			$regions_data = $this->db->getObjects($this->db->types[self::REGION_MARK], true);
		}
		return $regions_data;
	}

	function getRegionsAsDirectory($only_values = true){
		$regions_directory = null;
		if($regions_directory = @$this->db->app->directory[self::REGION_MARK]){
			$regions_directory->loaded or $this->db->LoadDirectory($regions_directory, null, false);
		}
		if($only_values && $regions_directory)
			$regions_directory = $regions_directory->parentField ? $regions_directory->getTree()->getPlainTree(null) : $regions_directory->getList();
		
		return $regions_directory;
	}

	function getMyRegionData(){
		$regions = $this->getRegionsData();
		$my_region = array();

		foreach($regions as $region){
			if($region['id'] == $this->rid){
				$my_region = $region;
				break;
			}
		}
		return $my_region;
	}

	function getRegionUrl($rid){
		foreach($this->getRegionsData() as $val){
			if($rid == $val['id']){
				$tmp_url = $val['url'];
				break;
			}
		}

		if(!isset($tmp_url)) return false;
		$tmp_url = str_replace(' ', '', $tmp_url);
		$urls = explode(',', $tmp_url);

		return reset($urls);
	}

	function getUserRid(){
		static $id;

		if(!isset($id)) {
			$host = preg_replace("#:\d{2,5}$#",'',$this->request->getHttpHost());
			$id   = 0;

			foreach($this->getRegionsData() as $region){
				if(!empty($region['url']) && $region['status']){
					$region['url'] = str_replace(' ', '', $region['url']);
					$regions       = explode(',', $region['url']);

					if(in_array($host, $regions)){
						$id = $region['id'];
						break;

					} else {
						foreach ($regions as $k => $r) {
							if (strstr($r, '*')) {
								$mask = str_replace('.', '\.', $r);
								$mask = str_replace('*', '[a-z0-9]+', $mask);
								
								if (preg_match('/'.$mask.'/i', $host)) {
									$id = $region['id'];
									break;
								}
							}
						}
					}

				}
			}
		}

	return $id;
	}

	function getAdminRid(){
		switch(true){
			case $this->isFederal():
				$this->setAdminRidFromRequest();
				if(isset($_SESSION['__current_region'])){
					$id = $_SESSION['__current_region'];
					break;
				}
			default:
				$id = $_SESSION['__current_region'] = $this->getUserRid();
		}
		
		return $id;
	}

	function getRid(){
		return $this->rid;
	}

	private function setAdminRidFromRequest(){
		$this->_doSession();
		if($this->request->isSetVar(self::REGION_MARK)){
			$rid = $this->request->getVar(self::REGION_MARK);
			if(isset($rid['rid'])){
				$_SESSION['__current_region'] = $rid['rid'];
			}
		}
	}
	
	function setRid(){
		$this->rid = $this->context == 'user' ? $this->getUserRid() : $this->getAdminRid();
	}

	function findByRegion(){
		return empty($this->rid) ? $this->db->TrueStatusString() : $this->db->inArray(self::REGION_MARK, $this->rid);
	}

	function isCurrentInSet($list){
		return empty($this->rid) || $this->isInRegion($this->rid, $list);
	}

	function isInRegion($rid, $list){
		return (is_array($list) && in_array($rid, $list, true));
	}

	function _doSession(){ //TODO: rewrite me plz, i'am ugly
		if(!isset($_SESSION)){
			session_name(self::REGION_MARK);
			ini_set('session.use_cookies',      1);
			ini_set('session.use_only_cookies', 1);
			ini_set('session.use_trans_sid',    0);
			session_start();
		}
	}

	//For triggers
	function isFederal(){
		$user = PXRegistry::getUser();
		return $user->can('manage_region');
	}

	function isRegionalObject($format){
		return isset($format->fields[PXMultiRegions::REGION_MARK]);
	}

	function addRegionByDefault($format, &$dbFields, &$dbValues){
		if($this->isRegionalObject($format)){
			$key = array_flip($dbFields);

			if(!$this->isFederal()){
				$reg = '{'.$this->rid.'}';
			} else {
				$reg = empty($dbValues[$key[self::REGION_MARK]]) ? '{'.$this->rid.'}' : $dbValues[$key[self::REGION_MARK]];
			}

			$dbValues[$key[self::REGION_MARK]] = $reg;
		}

		//TODO: Add general mask for suser
	}

	function checkRegionAccessToParent($format, $object){
		if(isset($object['parent'])){
			$parentFormat = $this->db->types[$format->parent];
			$parent = $this->db->GetObjectById($parentFormat, $object['parent']);
			if($this->isRegionalObject($parentFormat) && $parentFormat->id !== 'struct' && sizeof($parent)){ //check access to parent object if we try to add/modify/delete subcontent objects only
				return $this->checkRegionAccess($parentFormat, $parent);
			}
		}
		return true;
	}

	function checkRegionAccess($format, $object){
		if($this->isRegionalObject($format) && !$this->isFederal()){
			$user = PXRegistry::getUser();
			
			if(count($object[self::REGION_MARK]) != 1 || $this->rid != $object[self::REGION_MARK][0] || !$user->can('manage_region', null, array('id' => $this->rid))){
				return false;
			}
		}
		return true;
	}

}
