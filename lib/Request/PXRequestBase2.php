<?php
/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Description of PXRequestBase2
 *
 * @author vii
 */

class PXRequestBase2 {

    function getPath() {
        $separated_path = explode("/", $_SERVER['SCRIPT_NAME']);

        $filtered = array_filter($separated_path,
            create_function('$item', 'return (!empty($item) && !preg_match("#(index|default)\.#", $item));'));

        return array_values($filtered);
    }

    function getServerVar($name, $if_is_null=null) {
        return !is_null($value = $_SERVER[$name]) ? $value : $if_is_null;
    }

    function getHttpMethod() {
        return $this->getServerVar("REQUEST_METHOD", "CLI");
    }

    function getRequestUri() {
        return $this->getServerVar("REQUEST_URI");
    }

    function getXServerVars($xName, $name) {
        if (isset($_SERVER[$xName]))
            return trim(current(explode(",", $_SERVER[$xName])));

        return $this->getServerVar($name);
    }

    function getRemoteAddr() {
        return $this->getXServerVars("HTTP_X_REAL_IP", "REMOTE_ADDR");
    }

    function getHttpHost() {
        return $this->getXServerVars("HTTP_X_HOST", "HTTP_HOST");
    }

    function getVar($name, $if_is_null=null) {
        return !is_null($value = $_REQUEST[$name]) ? $value : $if_is_null;
    }

    function getCharset() {
        $charset = $this->getServerVar("CHARSET", "w");
        return strtolower($charset{0});
    }
}
?>
