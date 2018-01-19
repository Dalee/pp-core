<?php

require_once PPLIBPATH . 'Request/HttpVars/classes.inc';

use Ramsey\Uuid\Uuid;

class PXRequest {
	var $httpMethod;
	var $host;
	var $path;
	var $remoteAddr;
	var $getData;
	var $postData;
	var $cookieData;

	/** @var string unique request identifier */
	protected $uuid;

	protected $headers = [];

	protected $rootPathsList = array(
		'index.html',
		'index.shtml',
		'index.phtml',
		'index.php',
		'index.php3',
		'index.php4',
		'default.htm',
		'default.asp',
		'index.jsp'
	);

	function __construct() {
		$this->httpMethod = $this->GetHttpMethod();
		$this->host = $this->GetHttpHost();
		$this->remoteAddr = $this->GetRemoteAddr();
		$this->path = self::GetOriginalPath(); //PHP54/52: <static> is better but for 52 using <self>

		foreach ($this->getHttpHeaders() as $name => $value) {
			$this->headers[strtolower($name)] = $value;
		}

		$this->getData = new NLGetData();
		$this->postData = new NLPostData();
		$this->cookieData = new NLCookieData();
	}

	static function GetHttpMethod() {
		return (isset($_SERVER['REQUEST_METHOD'])) ? $_SERVER['REQUEST_METHOD'] : 'CLI';
	}

	static function GetHttpProto() {
		switch (true) {
			case isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] == 'on':
			case isset($_SERVER['HTTP_X_FORWARDED_PROTO']) && $_SERVER['HTTP_X_FORWARDED_PROTO'] == 'https':
				return "https";
			default:
				return "http";
		}
	}

	static function GetHttpReferer($default = NULL) {
		return self::getServerVar('HTTP_REFERER', $default);
	}

	static function GetRequestUri() {
		return (isset($_SERVER['REQUEST_URI'])) ? $_SERVER['REQUEST_URI'] : NULL;
	}

	function getReferer() {
		return self::GetHttpReferer($this->getData->_getVar('referer'));
	}

	static function GetOriginalPath() {
		// We need to use REQUEST_URI but not SCRIPT_NAME variable because SCRIPT_NAME is not available when mod_rewrite is enabling in .htaccess
		$parts = explode("?", $_SERVER['REQUEST_URI'], 2);
		return reset($parts);
	}

	function SetPath($path) {
		$this->path = $path;
	}

	function GetPathAsString() {
		return $this->path;
	}

	function IsBadPath() {
		$path = $this->GetPathAsString();
		// Terminal slash check
		if (preg_match('#\.(' . join('|', array_map('preg_quote', unserialize(ENUM_ALLOWED_PATHNAMES))) . ')/+$#' . REGEX_MOD, $path)) {
			return true;
		}
		// 'index.php' and etc. check (shouldn't occur in the middle)
		if (preg_match('#/(' . join('|', array_map('preg_quote', $this->rootPathsList)) . ')/#' . REGEX_MOD, $path)) {
			return true;
		}
		// All bytes in path should be valid utf-8 sequences
		if (!isUtfString($path)) {
			return true;
		}

		return false;
	}

	function GetPath() {
		$tmpArray = explode("/", $this->path);
		$path = array();
		foreach ($tmpArray as $tmp) {
			if (!mb_strlen($tmp) ||
				in_array(
					$tmp,
					$this->rootPathsList,
					true
				)
			) {
				continue;
			}

			$path[] = $tmp;
		}
		return $path;
	}

	static function GetHttpHost() {
		if (isset($_SERVER['HTTP_X_HOST'])) {
			if (mb_strstr($_SERVER['HTTP_X_HOST'], ",")) {
				$hosts = explode(",", $_SERVER['HTTP_X_HOST']);
				$host = trim(end($hosts));
			} else {
				$host = $_SERVER['HTTP_X_HOST'];
			}
		} elseif (isset($_SERVER['HTTP_HOST'])) {
			$host = $_SERVER['HTTP_HOST'];

		} else {
			return NULL;
		}

		return preg_replace('/:.+$/' . REGEX_MOD, '', $host);
	}

	protected function cleanHeaderName($name) {
		return str_replace(' ', '-', ucwords(strtolower(str_replace('_', ' ', substr($name, 5)))));
	}

	/**
	 * Keep this public as is not to make BC.
	 *
	 * @todo Need to return headers from this where keys are lower cased.
	 * @return array
	 */
	public function getHttpHeaders() {
		$headers = [];

		foreach ($_SERVER as $name => $value) {
			if (mb_substr($name, 0, 5) === 'HTTP_') {
				$headerName = $this->cleanHeaderName($name);
				$headers[$headerName] = $value;
			}
		}

		return $headers;
	}

	/**
	 * Looks up $headerName in headers (case insensitive!).
	 *
	 * @param string $headerName
	 * @param null $default
	 * @return mixed
	 */
	public function getHttpHeader($headerName, $default = null) {
		return getFromArray($this->headers, $headerName, $default);
	}

	static function GetRemoteAddr() {
		if (isset($_SERVER['HTTP_X_REAL_IP'])) {
			if (mb_strstr($_SERVER['HTTP_X_REAL_IP'], ",")) {
				$real_ip = explode(",", $_SERVER['HTTP_X_REAL_IP']);
				$real_ip = trim(end($real_ip));
			} else {
				$real_ip = $_SERVER['HTTP_X_REAL_IP'];
			}
			if (filter_var($real_ip, FILTER_VALIDATE_IP)) {
				return $real_ip;
			} else {
				// Hacking attempt via unsafe header?
				unset($_SERVER['HTTP_X_REAL_IP']);
				FatalError("Malformed IP address given in X-Real-Ip header !");
			}
		} elseif (isset($_SERVER['REMOTE_ADDR'])) {
			return $_SERVER['REMOTE_ADDR'];
		} else {
			return NULL;
		}
	}

	function getIpAddr($varName) {
		if ($this->getHttpMethod() == 'POST') {
			return $this->postData->_GetIPAddrVar($varName);
		} else {
			return $this->getData->_GetIPAddrVar($varName);
		}
	}

	function GetUserAgent() {
		return isset($_SERVER['HTTP_USER_AGENT']) ? $_SERVER['HTTP_USER_AGENT'] : '';
	}

	function GetGetVar($varName) {
		return $this->getData->_GetVar($varName);
	}

	function GetPostVar($varName) {
		return $this->postData->_GetVar($varName);
	}

	function GetCookieVar($varName) {
		return $this->cookieData->_GetVar($varName);
	}

	/**
	 * Set cookie
	 * @deprecated
	 */
	function SetCookieVar($varName, $value = '', $expire = NULL) {
		$response = PXRegistry::getResponse();
		return $response->SetCookie($varName, $value, $expire);
	}

	function GetAllGetData() {
		return $this->getData->GetAll();
	}

	function GetAllPostData() {
		return $this->postData->GetAll();
	}

	function GetAllCookieData() {
		return $this->cookieData->GetAll();
	}

	function _GetGetPostVar($varName) {
		switch ($this->httpMethod) {
			case 'POST':
				return $this->GetPostVar($varName);
			default:
				return $this->GetGetVar($varName);
		}
	}

	function _SetGetPostVar($varName, $value) {
		switch ($this->httpMethod) {
			case 'POST':
				return $this->postData->_SetVar($varName, $value);
			default:
				return $this->getData->_SetVar($varName, $value);
		}
	}

	function SetVar($varName, $value) {
		$functionName = 'Set' . mb_strtoupper($varName{0}) . mb_substr($varName, 1);

		if (method_exists($this, $functionName)) {
			$value = call_user_func(array(&$this, $functionName), $value);
		} else {
			$value = $this->_SetGetPostVar($varName, $value);
		}
		return $value;
	}

	function GetVar($varName, $default = NULL) {
		$functionName = 'Get' . ucfirst($varName);

		if (method_exists($this, $functionName)) {
			$value = call_user_func(array(&$this, $functionName));
		} else {
			$value = $this->_GetGetPostVar($varName);
		}

		return ($value !== NULL && $value !== '') ? $value : $default;
	}

	function isSetVar($varName) {
		switch ($this->httpMethod) {
			case 'POST':
				return $this->postData->_isSetVar($varName);
			default:
				return $this->getData->_isSetVar($varName);
		}
	}

	function GetUploadFile($varName, $isArray = FALSE) {
		return $this->postData->_GetFileVar($varName, $isArray);
	}

	/**
	 * Проверяет вхождение ip-адреса в подсеть.
	 *
	 * @param String $ip - ip-адрес, например 192.168.0.1
	 * @param String $subnet - подсеть, например 192.168.0.0/24
	 * @return Int
	 */
	static function IsIpInSubnet($ip, $subnet) {
		list($net, $mask) = explode("/", $subnet);

		$lnet = ip2long($net);
		$lip = ip2long($ip);
		$binnet = mb_str_pad(decbin($lnet), 32, "0", STR_PAD_LEFT);

		$firstpart = mb_substr($binnet, 0, $mask);
		$binip = mb_str_pad(decbin($lip), 32, "0", STR_PAD_LEFT);
		$firstip = mb_substr($binip, 0, $mask);

		return (int)(strcmp($firstpart, $firstip) === 0);
	}

	/**
	 * Проверяет вхождение ip-адреса в список подсетей.
	 *
	 * @param String $ip - ip-адрес, например 192.168.0.1
	 * @param Array $subnet - массив подсетей
	 * @return Int
	 */
	static function IsIpInSubnets($ip, $subnets) {
		$isIn = 0;

		foreach ($subnets as $s) {
			$isIn += PXRequest::IsIpInSubnet($ip, $s);
		}

		return $isIn;
	}

	// Функция возвраящает массив "открытых" в редакторском интерфейсе элементов дерева
	function getLeafStatus() {
		$leafStatus = $this->cookieData->_getVar('leafStatus');
		$openLeaf = array();
		if ($leafStatus) {
			foreach ($leafStatus as $l => $s) {
				if ($s == 'open') {
					$openLeaf[] = mb_substr($l, 6);
				}
			}
		}
		return $openLeaf;
	}

	// Возвращает текущий объект уровня "struct"
	function getSid() {
		return $this->_getGetPostVar('sid');
	}

	// Возвращает текущий объект уровня "content"
	function getCid() {
		return $this->_getGetPostVar('cid');
	}

	// Возвращаяет id редактируемого объекта
	function getId() {
		return $this->_getGetPostVar('id');
	}

	// Возвращаяет format редактируемого/добавляемого объекта
	function getFormat() {
		return $this->_getGetPostVar('format');
	}

	// Возвращаяет parent редактируемого/добавляемого объекта
	function getParent() {
		return $this->_getGetPostVar('parent');
	}

	// Возвращаяет действие производимое над редактируемым/добавляемым объектом
	function getAction() {
		return $this->_getGetPostVar('action');
	}

	// Возвращаяет тип текущего объекта
	function getCtype() {
		return $this->_getGetPostVar('ctype');
	}

	// POST
	function getLinks($reference) {
		$rawLinksData = $this->postData->_getArrayVar($reference->name, false);

		foreach ($rawLinksData as $r => $links) {
			foreach ($links as $rr => $link) {
				if (empty($link['fields'])) {
					continue;
				}

				foreach ($link['fields'] as $name => $field) {
					if (isset($reference->fields[$name])) {
						$object = array();
						$p = array('request' => $this);
						$lname = '__links_hack_fix_' . $name;
						$fakeField = (object)array('name' => $lname);
						$this->SetVar($lname, $field);
						$rawLinksData[$r][$rr]['fields'][$name] = $reference->fields[$name]->displayType->getFromRequest($fakeField, $object, $p);
					}
				}
			}
		}

		return $rawLinksData;
	}

	// POST
	function getContentObject($format) {
		$object = array();
		$p = array('request' => $this);

		foreach ($format->fields as $k => $v) {
			$object[$k] = $v->displayType->getFromRequest($v, $object, $p);
			$object[$k] = $v->storageType->getFromRequest($v, $object, $p);
		}

		return $object;
	}

	function getObjectSysVars($format, $neededVars = array()) {
		$object['id'] = $this->postData->_getNumericVar('id');

		if ($this->postData->_IsSet(OBJ_FIELD_CHILDREN) || in_array(OBJ_FIELD_CHILDREN, $neededVars)) {
			$object[OBJ_FIELD_CHILDREN] = $this->postData->_getArrayVar(OBJ_FIELD_CHILDREN);
		}

		//FIXME ! Obsoleted ?
		if ($this->postData->_IsSet('sys_accessmod')) {
			$object['sys_accessmod'] = $this->postData->_getNumericVar('sys_accessmod');
		}

		if ($this->postData->_IsSet('sys_accessput')) {
			$object['sys_accessput'] = $this->postData->_getNumericVar('sys_accessput');
		}

		return $object;
	}

	// GET/POST
	function getAck() {
		switch ($this->httpMethod) {
			case 'POST':
				return $this->postData->_getCheckBoxVar('ack');
			default:
				return $this->getData->_getCheckBoxVar('ack');
		}
	}

	// POST
	function getIniFileSettings() {
		return $this->postData->getIniFileSettings();
	}

	// POST
	function getAfterActionDeal() {
		return $this->postData->_IsSetVar('close') ? 'close' : 'back';
	}

	// Функция возвращает "раздел" редакторского интерфейса
	function getArea($default = NULL) {
		$area = $this->_getGetPostVar('area') ? $this->_getGetPostVar('area') : $this->getGetVar('area');
		return ($area == NULL) ? $default : $area;
	}

	// clean the fields for admin system
	function getOrderVar($format, $default_sort, $fields) {
		$currentOrder = $this->GetVar($format . '_order');
		if (!$currentOrder) {
			return $default_sort;
		}
		$order = array();
		$multipleOrders = explode(",", $currentOrder);
		foreach ($multipleOrders as $currentOrder) {
			list($field_name, $sort_dir) = array_pad(explode(" ", trim($currentOrder), 2), 2, '');
			$sort_dir = mb_strtoupper(trim($sort_dir)) == 'DESC' ? ' DESC' : '';
			if (!(isset($fields[$field_name]) && $fields[$field_name]->listed) && $field_name != 'sys_order') {
				continue;
			}

			$order[] = $field_name . $sort_dir;
		}
		return empty($order) ? $default_sort : join(",", $order);
	}

	/* From PXUserRequest */
	function getHostAndDir() {
		$a = $this->getPath();
		array_unshift($a, $this->getHttpHost());
		if (sizeof($a) > 1 && mb_strstr($a[sizeof($a) - 1], ".")) {
			array_pop($a);
		}
		return $a;
	}

	function getFile() {
		$a = $this->getPath();
		if (count($a)) {
			if (mb_strstr($a[sizeof($a) - 1], ".")) {
				return $a[sizeof($a) - 1];
			}
		}

		return 'index.html';
	}

	function isIndexFile($file = '') {
		return in_array(mb_strlen($file) ? $file : $this->getFile(), $this->rootPathsList, true);
	}

	function getPart() {
		$part = $this->_getGetPostVar('part');
		return is_null($part) ? 'index.html' : $part;
	}

	function isXmlHttpRequest() {
		return (bool)mb_stristr(self::getServerVar('HTTP_X_REQUESTED_WITH'), 'XMLHttpRequest');
	}

	static public function getServerVar($name, $default = null) {
		return isset($_SERVER[$name]) ? $_SERVER[$name] : $default;
	}

	/**
	 * Returns unique identifier for the request
	 *
	 * @return string
	 */
	public function getRequestId() {
		if (!$this->uuid) {
			$this->uuid = Uuid::uuid4()->toString();
		}
		return $this->uuid;
	}
}

?>
