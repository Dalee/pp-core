<?php

class PXRegistry {
	const unknownVarMessage = "Registry - not heap. Store your ? some where else!";

	private $app, $db, $request, $response, $user, $layout;
	private static $instance = null;

	//constructor
	private function __construct(){
	}

	public static function instance() {
		if(self::$instance == null)
			self::$instance = new PXRegistry();
		
		return self::$instance;
	}

	public static function canSaveIt($varName){
		return array_key_exists($varName, get_object_vars(self::instance()));
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
					throw new Exception(strtr(self::unknownVarMessage, "?", $varName));
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

	public static function assignToObject($object) {
		$registry_fields = array_keys(get_class_vars(__CLASS__));
		$object_fields   = array_keys(get_object_vars($object));
		$fields          = array_intersect($registry_fields, $object_fields);

		if(empty($fields)) return;

		foreach($fields as $field) {
			$registry_method = "get".ucfirst($field);
			$object->$field = PXRegistry::$registry_method();
		}
	}

	private static function set($varName, $variable) {
		$registry = self::instance();

		if(!self::canSaveIt($varName)) {
			throw new Exception(strtr(self::unknownVarMessage, "?", $varName));
		}

		$registry->$varName = $variable;
	}

	public static function setApp($app) {
		self::set('app', $app);
	}

	public static function setDB(PXDatabase $db) {
		self::set('db', $db);
	}

	public static function setRequest($request) {
		self::set('request', $request);
	}

	public static function setUser($user) {
		self::set('user', $user);
	}

	public static function setLayout($layout) {
		self::set('layout', $layout);
	}
}

?>
