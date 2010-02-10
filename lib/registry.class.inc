<?php
class PXRegistry {
	const unknownVarMessage = "Registry - not heap. Store your ? some where else!";

	private static $instance = null;

	private $app;
	private $db;
	private $request;
	private $response;
	private $user;
	private $layout;

	private function __construct(){
	}

	public static function instance() {
		if(self::$instance == null)
			self::$instance = new PXRegistry();

		return self::$instance;
	}

	public static function canSaveIt($varName){
		return in_array($varName, self::get_registry_fields());
	}

	private static function get($varName) {
		$registry = self::instance();

		if(!is_object($registry->$varName)) {
			switch($varName) {
				case 'app':
					throw new Exception("{$varName} wasn't set yet.");
					break;

				case 'db':
					$registry->$varName = new PXDataBase(self::getApp());
					break;

				case 'user':
					$registry->$varName = new PXUserAuthorized();
					break;

				case 'layout':
					$registry->$varName = new PXLayoutNull();
					break;

				case 'request':
					$registry->$varName = new PXRequest();
					break;

				case 'response':
					$registry->$varName = PXResponse::getInstance();
					break;

				default:
					throw new Exception(str_replace('?', $varName, self::unknownVarMessage));
					break;
			}
		}

		return $registry->$varName;
	}

	public static function getApp() {
		return self::get('app');
	}

	public static function getDb() {
		return self::get('db');
	}

	public static function getRequest() {
		return self::get('request');
	}

	public static function getUser() {
		return self::get('user');
	}

	public static function getLayout() {
		return self::get('layout');
	}

	public static function getResponse() {
		return self::get('response');
	}

	public static function getTypes($typeName=null) {
		static $types;

		if(!is_array($types)) {
			$app   = self::getApp();
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

	private static function get_registry_fields() {
		return array_keys(get_class_vars(__CLASS__));
	}

	public static function assignToObject($object) {
		$registry_fields = self::get_registry_fields();
		$object_fields   = array_keys(get_object_vars($object));
		$fields          = array_intersect($registry_fields, $object_fields);

		if(empty($fields)) return;

		foreach($fields as $field) {
			$registry_method = "get".ucfirst($field);
			$object->$field = self::$registry_method();
		}
	}

	private static
	function _set($varName, $variable) {
		$registry = self::instance();

		if(!self::canSaveIt($varName)) {
			throw new Exception(str_replace('?', $varName, self::unknownVarMessage));
		}

		$registry->$varName = $variable;
	}

	public static function setApp($app) {
		self::_set('app', $app);
	}

	public static function setDB(PXDatabase $db) {
		self::_set('db', $db);
	}

	public static function setRequest($request) {
		self::_set('request', $request);
	}

	public static function setUser($user) {
		self::_set('user', $user);
	}

	public static function setLayout($layout) {
		self::_set('layout', $layout);
	}
}
?>