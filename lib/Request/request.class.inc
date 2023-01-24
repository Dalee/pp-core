<?php

require_once PPLIBPATH . 'Request/HttpVars/classes.inc';

use Ramsey\Uuid\Uuid;

class PXRequest
{
	public $httpMethod;
	public $host;
	public $path;
	public $remoteAddr;
	public $getData;
	public $postData;
	public $cookieData;

	/** @var string unique request identifier */
	protected $uuid;

	protected $headers = [];

	protected $rootPathsList = [
		'index.html',
		'index.shtml',
		'index.phtml',
		'index.php',
		'index.php3',
		'index.php4',
		'default.htm',
		'default.asp',
		'index.jsp',
	];

	public function __construct()
	{
		$this->httpMethod = static::GetHttpMethod();
		$this->host = static::GetHttpHost();
		$this->remoteAddr = static::GetRemoteAddr();
		$this->path = static::GetOriginalPath();

		foreach ($this->getHttpHeaders() as $name => $value) {
			$this->headers[strtolower($name)] = $value;
		}

		$this->getData = new NLGetData();
		$this->postData = new NLPostData();
		$this->cookieData = new NLCookieData();
	}

	public static function GetHttpMethod()
	{
		return $_SERVER['REQUEST_METHOD'] ?? 'CLI';
	}

	public static function GetHttpProto()
	{
		return match (true) {
			isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] == 'on', isset($_SERVER['HTTP_X_FORWARDED_PROTO']) && $_SERVER['HTTP_X_FORWARDED_PROTO'] == 'https' => "https",
			default => "http",
		};
	}

	public static function GetHttpReferer($default = NULL)
	{
		return self::getServerVar('HTTP_REFERER', $default);
	}

	public static function GetRequestUri()
	{
		return $_SERVER['REQUEST_URI'] ?? NULL;
	}

	public function getReferer()
	{
		return self::GetHttpReferer($this->getData->_getVar('referer'));
	}

	public static function GetOriginalPath()
	{
		// We need to use REQUEST_URI but not SCRIPT_NAME variable because SCRIPT_NAME is not available when mod_rewrite is enabling in .htaccess
		$parts = explode("?", (string) $_SERVER['REQUEST_URI'], 2);
		return reset($parts);
	}

	public function SetPath($path)
	{
		$this->path = $path;
	}

	public function GetPathAsString()
	{
		return $this->path;
	}

	public function IsBadPath()
	{
		$path = $this->GetPathAsString();
		// Terminal slash check
		if (preg_match('#\.(' . join('|', array_map('preg_quote', ENUM_ALLOWED_PATHNAMES)) . ')/+$#' . REGEX_MOD, (string) $path)) {
			return true;
		}
		// 'index.php' and etc. check (shouldn't occur in the middle)
		if (preg_match('#/(' . join('|', array_map('preg_quote', $this->rootPathsList)) . ')/#' . REGEX_MOD, (string) $path)) {
			return true;
		}
		// All bytes in path should be valid utf-8 sequences
		if (!isUtfString($path)) {
			return true;
		}

		return false;
	}

	public function GetPath()
	{
		$tmpArray = explode("/", (string) $this->path);
		$path = [];
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

	public static function GetHttpHost()
	{
		if (isset($_SERVER['HTTP_X_HOST'])) {
			if (mb_strstr((string) $_SERVER['HTTP_X_HOST'], ",")) {
				$hosts = explode(",", (string) $_SERVER['HTTP_X_HOST']);
				$host = trim(end($hosts));
			} else {
				$host = $_SERVER['HTTP_X_HOST'];
			}
		} elseif (isset($_SERVER['HTTP_HOST'])) {
			$host = $_SERVER['HTTP_HOST'];

		} else {
			return NULL;
		}

		return preg_replace('/:.+$/' . REGEX_MOD, '', (string) $host);
	}

	protected function cleanHeaderName($name)
	{
		return str_replace(' ', '-', ucwords(strtolower(str_replace('_', ' ', substr((string) $name, 5)))));
	}

	/**
	 * Keep this public as is not to make BC.
	 *
	 * @return array
	 * @todo Need to return headers from this where keys are lower cased.
	 */
	public function getHttpHeaders()
	{
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
	public function getHttpHeader($headerName, $default = null)
	{
		return getFromArray($this->headers, $headerName, $default);
	}

	public static function GetRemoteAddr()
	{
		if (isset($_SERVER['HTTP_X_REAL_IP'])) {
			if (mb_strstr((string) $_SERVER['HTTP_X_REAL_IP'], ",")) {
				[$real_ip] = explode(",", (string) $_SERVER['HTTP_X_REAL_IP']);
				$real_ip = trim($real_ip);
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

	public function getIpAddr($varName)
	{
		if (static::getHttpMethod() == 'POST') {
			return $this->postData->_GetIPAddrVar($varName);
		} else {
			return $this->getData->_GetIPAddrVar($varName);
		}
	}

	public function GetUserAgent()
	{
		return $_SERVER['HTTP_USER_AGENT'] ?? '';
	}

	public function GetGetVar($varName)
	{
		return $this->getData->_GetVar($varName);
	}

	public function GetPostVar($varName)
	{
		return $this->postData->_GetVar($varName);
	}

	public function GetCookieVar($varName)
	{
		return $this->cookieData->_GetVar($varName);
	}

	/**
	 * Set cookie
	 * @deprecated
	 */
	public function SetCookieVar($varName, $value = '', $expire = NULL)
	{
		$response = PXRegistry::getResponse();
		return $response->SetCookie($varName, $value, $expire);
	}

	public function GetAllGetData()
	{
		return $this->getData->GetAll();
	}

	public function GetAllPostData()
	{
		return $this->postData->GetAll();
	}

	public function GetAllCookieData()
	{
		return $this->cookieData->GetAll();
	}

	public function _GetGetPostVar($varName)
	{
		return match ($this->httpMethod) {
			'POST' => $this->GetPostVar($varName),
			default => $this->GetGetVar($varName),
		};
	}

	public function _SetGetPostVar($varName, $value)
	{
		return match ($this->httpMethod) {
			'POST' => $this->postData->_SetVar($varName, $value),
			default => $this->getData->_SetVar($varName, $value),
		};
	}

	public function SetVar($varName, $value)
	{
		$functionName = 'Set' . mb_strtoupper((string) $varName[0]) . mb_substr((string) $varName, 1);

		if (method_exists($this, $functionName)) {
			$value = call_user_func([&$this, $functionName], $value);
		} else {
			$value = $this->_SetGetPostVar($varName, $value);
		}
		return $value;
	}

	public function GetVar($varName, $default = NULL)
	{
		$functionName = 'Get' . ucfirst((string) $varName);

		if (method_exists($this, $functionName)) {
			$value = call_user_func([&$this, $functionName]);
		} else {
			$value = $this->_GetGetPostVar($varName);
		}

		return ($value !== NULL && $value !== '') ? $value : $default;
	}

	public function isSetVar($varName)
	{
		return match ($this->httpMethod) {
			'POST' => $this->postData->_isSetVar($varName),
			default => $this->getData->_isSetVar($varName),
		};
	}

	public function GetUploadFile($varName, $isArray = FALSE)
	{
		return $this->postData->_GetFileVar($varName, $isArray);
	}

	/**
	 * Проверяет вхождение ip-адреса в подсеть.
	 *
	 * @param String $ip - ip-адрес, например 192.168.0.1
	 * @param String $subnet - подсеть, например 192.168.0.0/24
	 * @return Int
	 */
	public static function IsIpInSubnet($ip, $subnet)
	{
		[$net, $mask] = explode("/", $subnet);

		$lnet = ip2long($net);
		$lip = ip2long($ip);
		$binnet = mb_str_pad(decbin($lnet), 32, "0", STR_PAD_LEFT);

		$firstpart = mb_substr((string) $binnet, 0, $mask);
		$binip = mb_str_pad(decbin($lip), 32, "0", STR_PAD_LEFT);
		$firstip = mb_substr((string) $binip, 0, $mask);

		return (int)(strcmp($firstpart, $firstip) === 0);
	}

	/**
	 * Проверяет вхождение ip-адреса в список подсетей.
	 *
	 * @param String $ip - ip-адрес, например 192.168.0.1
	 * @param Array $subnet - массив подсетей
	 * @return Int
	 */
	public static function IsIpInSubnets($ip, $subnets)
	{
		$isIn = 0;

		foreach ($subnets as $s) {
			$isIn += PXRequest::IsIpInSubnet($ip, $s);
		}

		return $isIn;
	}

	// Функция возвраящает массив "открытых" в редакторском интерфейсе элементов дерева
	public function getLeafStatus()
	{
		$leafStatus = $this->cookieData->_getVar('leafStatus');
		$openLeaf = [];
		if ($leafStatus) {
			foreach ($leafStatus as $l => $s) {
				if ($s == 'open') {
					$openLeaf[] = mb_substr((string) $l, 6);
				}
			}
		}
		return $openLeaf;
	}

	// Возвращает текущий объект уровня "struct"
	public function getSid()
	{
		return $this->_getGetPostVar('sid');
	}

	// Возвращает текущий объект уровня "content"
	public function getCid()
	{
		return $this->_getGetPostVar('cid');
	}

	// Возвращаяет id редактируемого объекта
	public function getId()
	{
		return $this->_getGetPostVar('id');
	}

	// Возвращаяет format редактируемого/добавляемого объекта
	public function getFormat()
	{
		return $this->_getGetPostVar('format');
	}

	// Возвращаяет parent редактируемого/добавляемого объекта
	public function getParent()
	{
		return $this->_getGetPostVar('parent');
	}

	// Возвращаяет действие производимое над редактируемым/добавляемым объектом
	public function getAction()
	{
		return $this->_getGetPostVar('action');
	}

	// Возвращаяет тип текущего объекта
	public function getCtype()
	{
		return $this->_getGetPostVar('ctype');
	}

	// POST
	public function getLinks($reference)
	{
		$rawLinksData = $this->postData->_getArrayVar($reference->name, false);

		foreach ($rawLinksData as $r => $links) {
			foreach ($links as $rr => $link) {
				if (empty($link['fields'])) {
					continue;
				}

				foreach ($link['fields'] as $name => $field) {
					if (isset($reference->fields[$name])) {
						$object = [];
						$p = ['request' => $this];
						$lname = '__links_hack_fix_' . $name;
						$fakeField = (object)['name' => $lname];
						$this->SetVar($lname, $field);
						$rawLinksData[$r][$rr]['fields'][$name] = $reference->fields[$name]->displayType->getFromRequest($fakeField, $object, $p);
					}
				}
			}
		}

		return $rawLinksData;
	}

	// POST
	public function getContentObject($format)
	{
		$object = [];
		$p = ['request' => $this];

		foreach ($format->fields as $k => $v) {
			$object[$k] = $v->displayType->getFromRequest($v, $object, $p);
			$object[$k] = $v->storageType->getFromRequest($v, $object, $p);
		}

		return $object;
	}

	public function getObjectSysVars($format, $neededVars = [])
	{
		$object = [];
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
	public function getAck()
	{
		return match ($this->httpMethod) {
			'POST' => $this->postData->_getCheckBoxVar('ack'),
			default => $this->getData->_getCheckBoxVar('ack'),
		};
	}

	// POST
	public function getIniFileSettings()
	{
		return $this->postData->getIniFileSettings();
	}

	// POST
	public function getAfterActionDeal()
	{
		return $this->postData->_IsSetVar('close') ? 'close' : 'back';
	}

	// Функция возвращает "раздел" редакторского интерфейса
	public function getArea($default = NULL)
	{
		$area = $this->_getGetPostVar('area') ?: $this->getGetVar('area');
		return ($area == NULL) ? $default : $area;
	}

	// clean the fields for admin system
	public function getOrderVar($format, $default_sort, $fields)
	{
		$currentOrder = $this->GetVar($format . '_order');
		if (!$currentOrder) {
			return $default_sort;
		}
		$order = [];
		$multipleOrders = explode(",", (string) $currentOrder);
		foreach ($multipleOrders as $currentOrder) {
			[$field_name, $sort_dir] = array_pad(explode(" ", trim($currentOrder), 2), 2, '');
			$sort_dir = mb_strtoupper(trim((string) $sort_dir)) == 'DESC' ? ' DESC' : '';
			if (!(isset($fields[$field_name]) && $fields[$field_name]->listed) && $field_name != 'sys_order') {
				continue;
			}

			$order[] = $field_name . $sort_dir;
		}
		return empty($order) ? $default_sort : join(",", $order);
	}

	/* From PXUserRequest */
	public function getHostAndDir()
	{
		$a = $this->getPath();
		array_unshift($a, static::getHttpHost());
		if (sizeof($a) > 1 && mb_strstr((string) $a[sizeof($a) - 1], ".")) {
			array_pop($a);
		}
		return $a;
	}

	public function getFile()
	{
		$a = $this->getPath();
		if (is_countable($a) ? count($a) : 0) {
			if (mb_strstr((string) $a[sizeof($a) - 1], ".")) {
				return $a[sizeof($a) - 1];
			}
		}

		return 'index.html';
	}

	public function isIndexFile($file = '')
	{
		return in_array(mb_strlen((string) $file) ? $file : $this->getFile(), $this->rootPathsList, true);
	}

	public function getPart()
	{
		$part = $this->_getGetPostVar('part');
		return is_null($part) ? 'index.html' : $part;
	}

	public function isXmlHttpRequest()
	{
		return (bool)mb_stristr((string) self::getServerVar('HTTP_X_REQUESTED_WITH'), 'XMLHttpRequest');
	}

	public
	static function getServerVar($name, $default = null)
	{
		return $_SERVER[$name] ?? $default;
	}

	/**
	 * Returns unique identifier for the request
	 *
	 * @return string
	 */
	public function getRequestId()
	{
		if (!$this->uuid) {
			$this->uuid = Uuid::uuid4()->toString();
		}
		return $this->uuid;
	}
}
