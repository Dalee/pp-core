<?php
class PXMultiRegions {
	private $db;
	private $request;
	private $user;
	private $rid;
	private $context;

	private static $self;

	private static $regionsData    = array();
	private static $allowedRegions = array();

	private static $regionsDirectory;
	private static $isFederal;

	const REGION_MARK = 'sys_regions';
	const MAGIC       = -1;
	const MANAGE_REGION_MARK = 'manage_region';
	const DISPLAY_FIELD = 'title';

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

	function getRegionsData($asTree = false){/*{{{*/
		if(!isset(self::$regionsData[!!$asTree])){
			self::$regionsData[!!$asTree] = $this->db->getObjects($this->db->types[self::REGION_MARK], true, $asTree ? DB_SELECT_TREE : DB_SELECT_TABLE);
		}

		return self::$regionsData[!!$asTree];
	}/*}}}*/

	function getAllowedRegions() {/*{{{*/
		if (self::$allowedRegions) {
			return self::$allowedRegions;
		}

		self::$allowedRegions = $this->getRegionsData();

		foreach (self::$allowedRegions as $id => $object) {
			if (!$this->user->can(self::MANAGE_REGION_MARK, $this->format, $object)) {
				unset(self::$allowedRegions[$id]);
			}
		}

		unset(self::$allowedRegions[null]);
		return self::$allowedRegions;
	}/*}}}*/

	#fixme remove me
	function loadDirectory() {
		return $this->getAllowedRegions();
	}

	function substractCloned($regions, $cutOut=array()) {/*{{{*/
		return array_diff_key($regions, array_flip($cutOut));
	}/*}}}*/

	function isTree() {/*{{{*/
		return !empty($this->format->parent) && isset($this->format->fields['parent']);
	}/*}}}*/

	function getRegionsAsDirectory($only_values = true, $cutOutRegions=null) {/*{{{*/
		$regions = $this->loadDirectory();

		if ($cutOutRegions) {
			$regions = $this->substractCloned($regions, $cutOutRegions);
		}

		if (count($regions) == 1) {
			return $only_values ? getColFromTableWithIndexs($regions, self::DISPLAY_FIELD) : $regions;
		}

		if ($this->isTree()) {
			$regions = new NLTree($regions, 'id', 'parent', self::DISPLAY_FIELD, $saveOrphans=true);

			if ($only_values) {
				$regions = !empty($regions) ? $regions->getPlainTree(null) : array();
			}
		} else {
			$regions = getColFromTableWithIndexs($regions, self::DISPLAY_FIELD);
		}

		return $regions;
	}/*}}}*/

	function getMyRegionData(){/*{{{*/
		$regions = $this->getRegionsData();
		$my_region = array();

		foreach($regions as $region){
			if($region['id'] == $this->rid){
				$my_region = $region;
				break;
			}
		}
		return $my_region;
	}/*}}}*/

	function getRegionUrl($rid){/*{{{*/
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
	}/*}}}*/

	function getUserRid(){/*{{{*/
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

								if (preg_match('/^'.$mask.'$/i', $host)) {
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
	}/*}}}*/

	function getAdminRid() {/*{{{*/
		$id = null;
		$this->setAdminRidFromRequest();

		#previous same checks in the setAdminRidFromRequest are nessesary
		if (
			isset($_SESSION['__current_region'])
			&& ($this->inAllowed($_SESSION['__current_region']) // region in session allowed for current user
			|| $_SESSION['__current_region'] === '')            // or region in session is empty string (show all regions)
		) {
			$id = $_SESSION['__current_region'];
		}

		if (is_null($id)) {
			$id = $_SESSION['__current_region'] = $this->getRidFromLogin();
		}

		return $id;
	}/*}}}*/

	function getRidFromLogin() {/*{{{*/
		$urlRegionId = $this->getUserRid();

		if($this->inAllowed($urlRegionId)) {
			return $urlRegionId;
		}

		return "";
	}/*}}}*/

	function getRid(){/*{{{*/
		return $this->rid;
	}/*}}}*/

	private function setAdminRidFromRequest(){/*{{{*/
		$this->_doSession();

		if ($this->request->isSetVar(self::REGION_MARK)) {
			$rid = $this->request->getVar(self::REGION_MARK);

			if (!isset($rid['rid'])) {
				return;
			}

			$rid = $rid['rid'];

			if ((is_numeric($rid) && $this->inAllowed($rid)) || $rid === '') {
				$_SESSION['__current_region'] = $rid;
			}
		}
	}/*}}}*/

	function setRid() {/*{{{*/
		$this->rid = $this->context == 'user' ? $this->getUserRid() : $this->getAdminRid();
	}/*}}}*/

	function findByRegion($regionsList=null) {/*{{{*/
		$conditionArray = $regionsList
			? $regionsList
			: $this->getAllowedRegionsIds();

		return empty($this->rid) 
			? $this->db->intersectIntArray(PXMultiRegions::REGION_MARK, $conditionArray) 
			: $this->db->inArray(self::REGION_MARK, $this->rid);
	}/*}}}*/

	function isCurrentInSet($list){/*{{{*/
		return empty($this->rid) || $this->isInRegion($this->rid, $list);
	}/*}}}*/

	function isInRegion($rid, $list){/*{{{*/
		return (is_array($list) && in_array($rid, $list, true));
	}/*}}}*/

	function _doSession(){ //TODO: rewrite me plz, i'am ugly/*{{{*/
		if (!isset($_SESSION)) {
			session_name(self::REGION_MARK);
			ini_set('session.use_cookies',      1);
			ini_set('session.use_only_cookies', 1);
			ini_set('session.use_trans_sid',    0);
			session_start();
		}
	}/*}}}*/

	//For triggers
	function isFederal() {/*{{{*/
		if(self::$isFederal) {
			return self::$isFederal;
		}

		return self::$isFederal = $this->user->can('manage_region', $this->format);
	}/*}}}*/

	static function isRegionalObject($format){
		return isset($format->fields[PXMultiRegions::REGION_MARK]);
	}

	#fixme
	function addRegionByDefault($format, &$dbFields, &$dbValues, $modifiedObject=null) {
		if ($this->isRegionalObject($format)) {
			$allowedRegionsIds = $this->getAllowedRegionsIds();
			$key = array_flip($dbFields);

			$toArray =
				PXMultipleRegionsHelper::toArray($dbValues[$key[self::REGION_MARK]]);

			$reg = empty($toArray) || empty($toArray[0])
				? PXMultipleRegionsHelper::toString($allowedRegionsIds)
				: $dbValues[$key[self::REGION_MARK]];

			if ($modifiedObject) {
				$cutOutRegions = $this->getCutOutRegions($format, $modifiedObject);

				$regArray      = PXMultipleRegionsHelper::toArray($reg);
				$regArray      = array_diff($regArray, $cutOutRegions);

				$reg           = PXMultipleRegionsHelper::toString($regArray);
			}

			$dbValues[$key[self::REGION_MARK]] = $reg;
		}
	}

	function checkRegionAccessToParent($format, $object) {
		if (!PXRegistry::getApp()->isAdminEngine() // можем редактировать из части юзера
			|| $this->isRegionalObject($format)    // если объект региональный (значит мы уже прошли проверку на его доступность)
		) {
			return true;
		}

		if (isset($object['parent'])) {
			$parentFormat = $this->db->types[$format->parent];

			$parent = $this->db->GetObjectById($parentFormat, $object['parent']);

			# probably parent is hidden by regional filter
			if (!count($parent)) {
				return false;
			}

			# fixme check struct condition.
			# cloned object can delete not regional contents.
			if ($this->isRegionalObject($parentFormat)) { //check access to parent object if we try to add/modify/delete subcontent objects only
				return $this->checkRegionAccess($parentFormat, $parent);
			}
		}

		return true;
	}

	function checkRegionAccess($format, $object) {
		if (!$this->isRegionalObject($format)) {
			return true;
		}

		$regionsData = $this->getRegionsData();
		$orig        = array_flip($object[self::REGION_MARK]);
		$allowed     = array_flip($this->getAllowedRegionsIds());

		foreach ($orig as $id => $tmp) {
			if (!isset($regionsData[$id])) {
				unset($orig[$id]);
			}
		}

		return count($allowed) == count($orig + $allowed);
	}

	function getAllowedRegionsIds() {
		return array_keys($this->loadDirectory());
	}

	function getCountAllowedRegions() {
		return count($this->getAllowedRegionsIds());
	}

	function inAllowed($rid) {
		$regions = $this->getAllowedRegions();
		return isset($regions[$rid]);
	}

	public function getCutOutRegions($format, $object) {
		$reflex_field          = PXMultipleRegionsReflexer::REFLEX_FIELD;
		$regions_field         = self::REGION_MARK;

		$object[$reflex_field] = null;

		$getReflexId = 
			$this->db->query("select {$reflex_field} from {$format->id} where id = '{$object['id']}'");

		if (!empty($getReflexId)) {
			$object[$reflex_field] = reset(reset($getReflexId));
		}

		$pattern = '
			SELECT id, %1$s
				FROM %2$s
				WHERE %3$s = \'%4$s\'
				UNION
			SELECT id, %1$s
				FROM %2$s
				WHERE id = \'%4$s\'';

		$params = array(
			"pattern"         => $pattern,
			"regions"         => $regions_field,
			"format"          => $format->id,
			"condition_field" => $reflex_field,
			"condition_value" => isset($object[$reflex_field]) ? $object[$reflex_field] : $object['id']
		);

		$sql = call_user_func_array("sprintf", $params);

		$result = $this->db->query($sql);
		$result = array_combine(getColFromTable($result, 'id'), $result);

		unset($result[$object['id']]);

		$cutOut = array_map(
			array("PXMultipleRegionsHelper", "toArray"), 
			getColFromTable($result, $regions_field)
		);

		$tmp = array();
		foreach ($cutOut as $i) {
			$tmp = array_merge($tmp, $i);
		}
		$cutOut = $tmp;

		return $cutOut;
	}
}
