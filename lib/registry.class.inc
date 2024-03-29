<?php

use Monolog\Registry;
use PP\Lib\Html\Layout;
use PP\Lib\Database\DatabaseInterface;
use PP\Lib\Database\Driver\PostgreSqlDriver;
use PP\Lib\Http\Response;

final class PXRegistry {
	public const unknownVarMessage = "Registry - not heap. Store your %s somewhere else!";

	private static ?\PXRegistry $instance = null;

	private $app;
	private $db;
	private $request;
	private $response;
	private $user;
	private $layout;
	private $struct;
	private $engine;

	/**
	 * @var null|\Symfony\Component\HttpFoundation\Session\SessionInterface
	 */
	private $session;

	private readonly \PXRegistryUSER $userRegistry;

	private function __construct(){
		$this->userRegistry = new PXRegistryUSER();
	}

	private static function instance() {
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
				case 'engine':
				case 'app':
					throw new Exception("{$varName} wasn't set yet.");

				case 'db':
					$registry->$varName = new PXDatabase(self::getApp());
					break;

				case 'user':
					$registry->$varName = new PXUserAuthorized();
					break;

				case 'layout':
					$registry->$varName = new Layout\NullLayout();
					break;

				case 'request':
					$registry->$varName = new PXRequest();
					break;

				case 'response':
					$registry->$varName = Response::getInstance();
					break;

				case 'struct':
					$registry->$varName  = self::getDB()->getObjects(self::getApp()->types['struct'], null, DB_SELECT_TREE);
					break;

				case 'session':
					//TODO: should we initialize some default session handler here?
					$registry->$varName = null;
					break;

				default:
					throw new Exception(sprintf(self::unknownVarMessage, $varName));
			}
		}

		return $registry->$varName;
	}

	/**
	 * @static
	 * @return \PP\Lib\Engine\AbstractEngine
	 */
	public static function getEngine() {
		return self::get('engine');
	}

	/**
	 * @static
	 * @return PXApplication
	 */
	public static function getApp() {
		return self::get('app');
	}

	/**
  * @static
  */
 public static function getDb(): \PXDatabase|\PP\Lib\Database\Driver\PostgreSqlDriver {
		return self::get('db');
	}

	/**
	 * @static
	 * @return PXRequest
	 */
	public static function getRequest() {
		return self::get('request');
	}

	/**
	 * @static
	 * @return PXUserAuthorized
	 */
	public static function getUser() {
		return self::get('user');
	}

	/**
	 * @return Layout\NullLayout|PXUserHTMLLayout
	 */
	public static function getLayout() {
		return self::get('layout');
	}

	/**
	 * @static
	 * @return Response
	 */
	public static function getResponse() {
		return self::get('response');
	}

	public static function getStruct() {
		return self::get('struct');
	}

	/**
	 * @return null|\Symfony\Component\HttpFoundation\Session\SessionInterface
	 */
	public static function getSession() {
		return self::get('session');
	}

	/**
	 * @param string $name
	 * @return Monolog\Logger
	 * @throws Exception
	 */
	public static function getLogger($name = LOGGER_APP) {
		return Registry::getInstance($name);
	}

	public static function getTypes($typeName = null) {
		static $types;

		if(!is_array($types)) {
			$app   = self::getApp();
			$types = $app->types;
		}

		if(!is_null($typeName)) {
			return $types[$typeName] ?? null;
		}

		return $types;
	}

	public static function getHKCU() {
		return self::instance()->userRegistry;
	}

	private static function get_registry_fields() {
		return array_keys(get_class_vars(self::class));
	}

	public static function assignToObject($object) {
		$registry_fields = self::get_registry_fields();
		$object_fields   = array_keys(get_object_vars($object));
		$fields          = array_intersect($registry_fields, $object_fields);

		if(empty($fields)) return;

		foreach($fields as $field) {
			$registry_method = "get".ucfirst((string) $field);
			$object->$field = self::$registry_method();
		}
	}

	private static function _set($varName, $variable) {
		$registry = self::instance();

		if(!self::canSaveIt($varName)) {
			throw new Exception(sprintf(self::unknownVarMessage, $varName));
		}

		$registry->$varName = $variable;
	}

	public static function setEngine($engine) {
		self::_set('engine', $engine);
	}

	public static function setApp($app) {
		self::_set('app', $app);
	}

	public static function setDB(DatabaseInterface $db) {
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

	public static function setSession($session) {
		self::_set('session', $session);
	}
}

final class PXRegistryUSER
{
	private ?array $__hash__ = null;

	public
	function get($key)
	{
		if (isset($this->__hash__[$key])) {
			return $this->__hash__[$key];
		}
	}

	public function &getByRef($key)
	{
		if (isset($this->__hash__[$key])) {
			return $this->__hash__[$key];
		}
	}

	public function set($key, $value, $overwrite = true)
	{
		if (!isset($this->__hash__[$key]) || $overwrite) {
			$this->__hash__[$key] = $value;
		}
	}

	// Overloading
	public function __get($name)
	{
		return $this->get($name);
	}

	public function __isset($name)
	{
		return isset($this->__hash__[$name]);
	}
}
