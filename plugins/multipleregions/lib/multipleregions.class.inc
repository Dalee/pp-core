<?php
class PXMultiRegions {
	private $db;
	private $request;
	private $user;
	private $rid;
	private $context;

	private static $self;
	private static $regionsData;
	private static $regionsDirectory;
	private static $isFederal;

	public $singleObject = false;

	const REGION_MARK = 'sys_regions';
	const MAGIC       = -1;

	private function __construct(){/*{{{*/
		$this->db      = PXRegistry::getDb();
		$this->request = PXRegistry::getRequest();
		$this->user    = PXRegistry::getUser();

		$this->format  = PXRegistry::getApp()->types[self::REGION_MARK];
	}/*}}}*/

	function getInstance($context = NULL){ //context: ENUM {"user", "admin"}/*{{{*/
		if(empty(self::$self)){
			$klass = __CLASS__;
			self::$self = new $klass();
		}

		if($context){
			self::$self->context = $context;
		}

		return self::$self;
	}/*}}}*/

	function getRegionsData(){/*{{{*/
		if(!isset(self::$regionsData)){
			self::$regionsData = $this->db->getObjects($this->db->types[self::REGION_MARK], true);
		}

		return self::$regionsData;
	}/*}}}*/

	function loadDirectory() {/*{{{*/
		if(self::$regionsDirectory) {
			return self::$regionsDirectory;
		}

		self::$regionsDirectory = PXRegistry::getApp()->directory[self::REGION_MARK];
		self::$regionsDirectory->loaded or $this->db->LoadDirectory(self::$regionsDirectory, null, false);

		foreach(self::$regionsDirectory->values as $id => $r) {
			if(!$this->user->can('manage_region', $this->format, $r)) {
				unset(self::$regionsDirectory->values[$id]);
			}
		}

		unset(self::$regionsDirectory->values[null]);
		return self::$regionsDirectory;
	}/*}}}*/

	function getRegionsAsDirectory($only_values = true, $location=null) {/*{{{*/
		$regionsDirectory = $this->loadDirectory();

		#delete already cloned regions
		if($location && is_array($location)) {
			$location = array_flip($location);
			foreach($regionsDirectory->values as $id => $region) {
				if(isset($location[$id])) {
					unset($regionsDirectory->values[$id]);
				}
			}
		}

		if(count($regionsDirectory->values) == 1) {
			$region = pos($regionsDirectory->values);
			$this->singleObject = true;
			return array($region['id'] => $region['title']);
		}

		if($regionsDirectory->parentField) {
			$regionsTree = $regionsDirectory->getTree();
			$regionsDirectory = !empty($regionsTree) ? $regionsTree->getPlainTree(null) : array();
		} else {
			$regionsDirectory = $regionsDirectory->getList();
		}

		return $regionsDirectory;
	}/*}}}*/

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
									break(2);
								}
							}
						}
					}
				}
			}
		}

		return $id;
	}

	function getAdminRid() {
		$id = null;
		$this->setAdminRidFromRequest();

		if(isset($_SESSION['__current_region'])){
			$id = $_SESSION['__current_region'];
		}

		if(!$id && intval($id)) {
			$id = $_SESSION['__current_region'] = $this->getRidFromLogin();
		}

		return $id;
	}
	// FIXME !
	function getRidFromLogin() {
		$regions = $this->db->query('SELECT objectid, objectparent FROM acl_objects 
			WHERE sgroupid IN ('.implode(',', $this->user->groups).') AND objectid IS NOT null');

		foreach ($regions as $k => $r) {
			$rIds[] = $r['objectid'];
		}

		$urlRegion = $this->getUserRid();
		switch (count($rIds)) {
			case 0:
				return $urlRegion;
			case 1:
				return current($rIds);
			default:
				return (!in_array($urlRegion, $rIds) ? current($rIds) : $urlRegion);
		}
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

	function setRid() {
		$this->rid = $this->context == 'user' ? $this->getUserRid() : $this->getAdminRid();
	}

	function findByRegion($regionsList=null) {
		$conditionArray = $regionsList
			? $regionsList
			: array_keys($this->loadDirectory()->values);

		$sqlFilter = sprintf(" sys_regions && '%s' ", PXMultiRegions::regToString($conditionArray));
		return empty($this->rid) ? $sqlFilter : $this->db->inArray(self::REGION_MARK, $this->rid);
	}

	// REMOVEME
	function exceptNull($array) {
		unset($array[""]);
		return $array;
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
	function isFederal() {
		if(self::$isFederal) {
			return self::$isFederal;
		}

		return self::$isFederal = $this->user->can('manage_region', $this->format);
	}

	static function isRegionalObject($format){
		return isset($format->fields[PXMultiRegions::REGION_MARK]);
	}

	#fixme
	function addRegionByDefault($format, &$dbFields, &$dbValues){
		if($this->isRegionalObject($format)){
			$allowedRegions = array_keys($this->loadDirectory()->values);
			$key = array_flip($dbFields);

			$toArray = 
				PXMultiRegions::regToArray($dbValues[$key[self::REGION_MARK]]);

			$reg = empty($toArray) || empty($toArray[0])
				? PXMultiRegions::regToString($allowedRegions)
				: $dbValues[$key[self::REGION_MARK]];

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

	function checkRegionAccess($format, $object) {
		if(!$this->isRegionalObject($format)) {
			return true;
		}

		$orig  = array_flip($object[self::REGION_MARK]);
		$allowed = array_flip(array_keys($this->loadDirectory()->values));

		#fixme clean up database!
		unset($orig["NULL"]);
		unset($orig[-1]);
		unset($orig[0]);

		$count = count($allowed);
		$merge = $orig + $allowed;

		return $count == count($merge);
	}

	#todo: move to helper
	static function regToArray($regString) {
		return array_filter(explode(",", strtr($regString, array("{" => "", "}" => ""))), "is_numeric");
	}

	static function regToString($regArray) {
		return sprintf("{%s}", join(",", array_filter($regArray, "is_numeric")));
	}
}
