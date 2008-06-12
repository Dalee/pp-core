<?
class PXRegistry {
	private static $app, $db, $user, $request, $layout, $vars, $types;

	public static function &getApp() {
		if(!is_object(self::$app) || is_null(self::$app)) {
			self::$app = PXApplication::getInstance(BASEPATH);
		}

		return self::$app;
	}

	public static function &getDb() {
		if(!is_object(self::$db) || is_null(self::$db)) {
			self::$db = new PXDataBase(self::getApp());
		}

		return self::$db;
	}

	public static function &getRequest() {
		if(!is_object(self::$request) || is_null(self::$request)) {
			self::$request = new PXRequest();
		}

		return self::$request;
	}

	public static function &getUser() {
		if(!is_object(self::$user) || is_null(self::$user)) {
			self::$user = new PXUserAuthorized();
		}

		return self::$user;
	}

	public static function &getLayout() {
		if(!is_object(self::$layout) || is_null(self::$layout)) {
			self::$layout = new PXLayoutNull();
		}

		return self::$layout;
	}

	public static function &getTypes($typeName=null) {
		if(!is_array(self::$types) || is_null(self::$types)) {
			self::$types =& self::getApp()->types;
		}

		if(!is_null($typeName)) {
			if(isset(self::$types[$typeName])) {
				return self::$types[$typeName];

			} else {
				$nullType = null;
				return $nullType;
			}
		}

		return self::$types;
	}

	public static function setLayout(&$layout) {
		self::$layout =& $layout;
	}

	public static function setVar($varName, &$varValue) {
		self::$vars[$varName] =& $varValue;
	}

	public static function &getVar($varName, $defaultValue=null) {
		if(isset(self::$vars[$varName])) {
			return self::$vars[$varName];
		}

		return $defaultValue;
	}
}
?>