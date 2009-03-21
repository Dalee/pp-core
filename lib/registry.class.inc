<?php
class PXRegistry {
	// private
	var $app;
	var $db;
	var $request;
	var $response;
	var $user;
	var $layout;
	
	//constructor
	private function __construct(){
	}

	private static function &instance() {
		static $registry;

		if(!is_object($registry)) {
			$registry = new PXRegistry();
		}

		return $registry;
	}

	private static function canSaveIt($varName){
		$registry =& self::instance();
		return array_key_exists($varName, get_object_vars($registry));
	}

	private static function &_get($varName) {
		$registry =& self::instance();

		if(!is_object($registry->$varName)) {
			switch($varName) {
				case 'app':
					$registry->_varIsNotSetException($varName);
					break;

				case 'db':
					$registry->$varName = & new PXDataBase(self::getApp());
					break;

				case 'user':
					$registry->$varName = & new PXUserAuthorized();
					break;

				case 'layout':
					$registry->$varName = & new PXLayoutNull();
					break;

				case 'request':
					$registry->$varName = & new PXRequest();
					break;

				case 'response':
					$registry->$varName = & PXResponse::getInstance();
					break;

				default:
					$registry->_unknownVarException($varName);
					break;
			}
			
		}

		return $registry->$varName;
	}

	public static function &getApp() {
		return self::_get('app');
	}

	public static function &getDb() {
		return self::_get('db');
	}

	public static function &getRequest() {
		return self::_get('request');
	}

	public static function &getUser() {
		return self::_get('user');
	}

	public static function &getLayout() {
		return self::_get('layout');
	}

	public static function &getTypes() {
		$app =& self::getApp();

		return $app->types;
	}

	/*function &getTypes($typeName=null) {
		if(!is_array($types)) {
			$app =  self::getApp();
			$types = $app->types;
		}

		if(!is_null($typeName)) {
			if(isset($types[$typeName])) {
				return $types[$typeName];

			} else {
				$nullType = null;
				return $nullType;
			}
		}
		return $types;
	}
	*/

	private static function _set($varName, &$variable) {
		$registry =& self::instance();
		if(!self::canSaveIt($varName)) {
			$registry->_unknownVarException($varName);
		}

		$registry->$varName = & $variable;
	}

	public static function setApp(&$app) {
		self::_set('app', $app);
	}

	public static function setDB(&$db) {
		self::_set('db', $db);
	}

	public static function setRequest(&$request) {
		self::_set('request', $request);
	}

	public static function setUser(&$user) {
		self::_set('user', $user);
	}

	public static function setLayout(&$layout) {
		self::_set('layout', $layout);
	}

	function _unknownVarException($e){
		FatalError("Registry - not heap. Store your {$e} some where else!");
	}

	function _varIsNotSetException($e){
		FatalError("{$e} wasn't set yet.");
	}
}
?>
