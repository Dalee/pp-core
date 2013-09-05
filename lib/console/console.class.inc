<?php
/**
 * console worker
 *
 * @version	0.1
 * @package	console
 *
 * @author		alex <alex@qfox.ru> yaroshevich
 */

require_once PPLIBPATH . 'Common/functions.console.inc';

/**
 * console data wrapper
 *
 * @method mixed command()
 * @method mixed flag()
 * @method mixed param()
 * @method mixed args()
 */
class PXConsole {

	// singleton, yeah
	private static $instance;
	public static function getInstance () {
		self::$instance || self::$instance = new self();
		return self::$instance;
	}
	private function __clone() {
		// deny cloning
	}


	protected static $args = array();
	protected static $aliases = array(
		'debug' => 'd',
		'verbose' => 'v'
	);

	protected static $command = null;
	protected static $data = array(
		'flags'   => array(),
		'options' => array(),
		'params'  => array(),
	);

	protected function __construct() {
	}

	public static function init($args) {
		static::set_args($args);
		static::getInstance();
	}

	public static function set_args($args) {
		is_string($args) && $args = con_parse_args_string($args);
		static::$args = $args;
		static::$data = con_parse_args(static::$args, static::$aliases);
		static::$command = reset(static::$data['params']);
	}

	public static function use_aliases(array $aliases) {
		static::$aliases = $aliases;
		static::$data = con_parse_args(static::$args, static::$aliases);
		static::$command = reset(static::$data['params']);
	}

	/**
	 * Default call handler
	 */
	public function __call($name, $args) {
		static $getters = array('f'=>'flag', 'o'=>'option', 'p'=>'param');

		switch (true) {
			case $name == 'command':
				return static::$command;

			case isset(static::$data[$name]):
				return static::$data[$name];

			case isset($getters[$name]):
				$name = $getters[$name];
			case in_array($name, $getters):
				array_unshift($args, $name);
				return call_user_func_array(array($this, '_get'), $args);

			default:
				throw new Exception(__CLASS__ . ' don\'t have a ' . $name . ' method');
		}
	}

	/**
	 * Default static call handler
	 */
	public static function __callStatic($name, $args) {
		return static::getInstance()->__call($name, $args);
	}

	/**
	 * Handles flags(), options() and args()  methods
	 */
	protected function _get($name, $key, $default = null) {

		if (is_array($key)) {
			$r = array();
			foreach ($key as $i => $k) {
				$rk = is_scalar($k) ? $k : $i;
				$r[$rk] = $this->_get($name, $k);
			}
			return $r;
		}

		$var = $key;
		$name = rtrim($name, 's') . 's';

		if (is_scalar($var) && isset(static::$data[$name]) && isset(static::$data[$name][$var])) {
			return static::$data[$name][$var];
		}

		return $default;
	}

	public function __get($k) {
		if (isset(self::$$k)) {
			return self::$$k;
		}
		if (false !== strpos($k, '_')) {
			list ($name, $key) = explode('_', $key, 1);
			return $this->__call($name, $key);
		}
		foreach (static::$data as $name => $items) {
			if (isset($items[$k])) {
				return $this->__call($name, $key);
			}
		}
	}

}
