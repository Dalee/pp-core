<?php
require_once('httpvars.classes.inc');

class NLRequest {
    var $httpMethod;
    var $host;
    var $path;
    var $remoteAddr;
    var $getData;
    var $postData;
    var $cookieData;

    function NLRequest() {
        $this->httpMethod = $this->GetHttpMethod();
        $this->host       = $this->GetHttpHost();
        $this->remoteAddr = $this->GetRemoteAddr();
        $this->path       = $this->GetOriginalPath();
        $this->getData    = new NLGetData();
        $this->postData   = new NLPostData();
        $this->cookieData = new NLCookieData();
    }

    static function GetHttpMethod() {
        return (isset($_SERVER['REQUEST_METHOD'])) ? $_SERVER['REQUEST_METHOD'] : 'CLI';
    }

    static function GetHttpReferer() {
        return (isset($_SERVER['HTTP_REFERER'])) ? $_SERVER['HTTP_REFERER'] : NULL;
    }

    static function GetRequestUri() {
        return (isset($_SERVER['REQUEST_URI'])) ? $_SERVER['REQUEST_URI'] : NULL;
    }

    function GetReferer() {
        if (isset($_SERVER['HTTP_REFERER'])) {
            return $_SERVER['HTTP_REFERER'];
        } elseif ($this->getData->_GetVar('referer')) {
            return $this->getData->_GetVar('referer');
        } else {
            return NULL;
        }
    }

    static function GetOriginalPath() {
        return $_SERVER['SCRIPT_NAME'];
    }

    function GetPath() {
        $tmpArray = explode("/", $this->path);
        $path = array();
        foreach ($tmpArray as $tmp) {
            if (empty($tmp)          ) continue;

            if ($tmp == 'index.html' ) continue;
            if ($tmp == 'index.shtml') continue;
            if ($tmp == 'index.phtml') continue;
            if ($tmp == 'index.php'  ) continue;
            if ($tmp == 'index.php3' ) continue;
            if ($tmp == 'index.php4' ) continue;
            if ($tmp == 'default.htm') continue;
            if ($tmp == 'default.asp') continue;
            if ($tmp == 'index.jsp'  ) continue;

            $path[] = $tmp;
        }
        return $path;
    }

    static function GetHttpHost() {
        if (isset($_SERVER['HTTP_X_HOST'])) {
            $http_x_host = $_SERVER['HTTP_X_HOST'];
            return strstr($http_x_host, ",") ? 
                    trim(substr($http_x_host, strrpos($http_x_host, ',') + 1)) : $http_x_host;

        } elseif (isset($_SERVER['HTTP_HOST'])) {
            return $_SERVER['HTTP_HOST'];
        } else {
            return NULL;
        }
    }

    static function GetRemoteAddr() {
        if (isset($_SERVER['HTTP_X_REAL_IP'])) {
            $http_x_real_ip = $_SERVER['HTTP_X_REAL_IP'];

            return strstr($http_x_real_ip, ",") ?
                 trim(substr($http_x_real_ip, strrpos($http_x_real_ip, ',') + 1)) : $http_x_real_ip;

        } elseif (isset($_SERVER['REMOTE_ADDR'])) {
            return $_SERVER['REMOTE_ADDR'];
        } else {
            return NULL;
        }
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

    function SetCookieVar($varName, $value='', $expire=NULL) {
        $this->cookieData->_SetVar('charcheck', 'йцукен', $expire);
        return $this->cookieData->_SetVar($varName, $value, $expire);
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
            case 'POST': return $this->GetPostVar($varName);
            default:     return $this->GetGetVar($varName);
        }
    }

    function _SetGetPostVar($varName, $value) {
        switch ($this->httpMethod) {
            case 'POST': return $this->postData->_SetVar($varName, $value);
            default:     return $this->getData->_SetVar($varName, $value);
        }
    }

    function SetVar($varName, $value) {
        $functionName = "Set$varName";

        if (strtolower($varName) != 'var' and method_exists($this, $functionName)) {
            $value = $this->$functionName($value);
        } else {
            $value = $this->_SetGetPostVar($varName, $value);
        }
        return $value;
    }

    function GetVar($varName, $default = NULL) {
        $functionName = "Get$varName";

        if (strtolower($varName) != 'var' and method_exists($this, $functionName)) {
            $value = $this->$functionName();
        } else {
            $value = $this->_GetGetPostVar($varName);
        }
        return ($value !== NULL && $value !== '') ? $value : $default;
    }

    function isSetVar($varName) {
        switch ($this->httpMethod) {
            case 'POST': return $this->postData->_isSetVar($varName);
            default:     return $this->getData->_isSetVar($varName);
        }
    }

    function GetUploadFile($varName, $isArray=FALSE) {
        return $this->postData->_GetFileVar($varName, $isArray);
    }

    /**
     * Проверяет вхождение ip-адреса в подсеть.
     *
     * @param String $ip        - ip-адрес, например 192.168.0.1
     * @param String $subnet    - подсеть, например 192.168.0.0/24
     * @return Int
     */
    static function IsIpInSubnet($ip, $subnet) {
        list($net,$mask) = split("/",$subnet);

        $lnet   = ip2long($net);
        $lip    = ip2long($ip);
        $binnet = str_pad(decbin($lnet), 32, "0", STR_PAD_LEFT);

        $firstpart = substr($binnet, 0, $mask);
        $binip     = str_pad(decbin($lip), 32, "0", STR_PAD_LEFT);
        $firstip   = substr($binip, 0, $mask);

        return (int)(strcmp($firstpart, $firstip) === 0);
    }

    /**
     * Проверяет вхождение ip-адреса в список подсетей.
     *
     * @param String $ip        - ip-адрес, например 192.168.0.1
     * @param Array  $subnet    - массив подсетей
     * @return Int
     */
    static function IsIpInSubnets($ip, $subnets) {
        $isIn = 0;

        foreach($subnets as $s) {
            $isIn += NLRequest::IsIpInSubnet($ip, $s);
        }

        return $isIn;
    }
}
?>
