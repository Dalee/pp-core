<?php

class PXEngineCommand extends PXEngineSbin {

	const COMMAND_EXTENSION = 'command.inc';

	static $con;
	static $processing_action;
	static $commands;

	public function __construct () {
		parent::__construct();

		require_once(PPLIBPATH . 'console/console.class.inc');
		require_once(PPLIBPATH . 'console/command.class.inc');
		require_once(PPLIBPATH . 'console/commandmethoddescription.class.inc');

		static::$con = PXConsole::getInstance();
	}

	public static function run($command = null, array $args = array()) {
		!empty($args)   && static::$con->set_args($args);
		empty($command) && $command = static::$con->command;

		$action = static::search($command);

		if (!$action) {
			return static::run('list');
		}

		try {
			$command = static::command($action);
			static::$processing_action = $action;

			$r = static::process($action);

			static::$processing_action = null;
		} catch (\Exception $e) {
			con_alert($e->getMessage());
			if (isset($e->xdebug_message)) {
				con_debug($e->xdebug_message);
			}
			die;
		}

		static::benchmarks();

		return $r;
	}

	protected static function search($command) {
		list ($ns, $command, $method) = array_pad(array_pad(explode(':', $command), 2, ''), -3, null);

		// var_dump(pp_base_paths('commands', true)); //, $command . '.' . self::COMMAND_EXTENSION));
		if ($ns) {
			$path = pp_plugin_path($ns, 'commands'); // broken
		} else {
			$path = pp_base_paths('commands', true);
		}
		$file = pp_fetch_file($path, $command . '.' . self::COMMAND_EXTENSION);

		if ($file) {
			$class = "PXCommand".$command;
		} elseif (is_callable('self::command_'.$command)) {
			$method = 'command_'.$command;
			$class = 'self';
		} else {
			return false;
		}

		return compact('ns', 'command', 'class', 'method', 'file', 'found');
	}

	protected static function fetch_commands() {
		$paths = array_filter(array_map(function ($path) {
			return file_exists($path) ? glob($path.'*.'.self::COMMAND_EXTENSION) : null;
		}, pp_base_paths('commands', true)));
		$commands = array();
		$commands[] = 'list'; // static::command_list
		array_walk_recursive($paths, function ($v, $k) use (&$commands) {
			$commands[] = str_replace('.' . self::COMMAND_EXTENSION, '', pathinfo($v, PATHINFO_BASENAME));
		});
		return $commands;
	}

	protected static function command($action) {
		is_string($action) && $action = static::search($action);

		if (!$action['file'] && $action['class'] != 'self') {
			static::$commands[$action['command']] = false;
		}
		if (isset($commands[$action['command']])) {
			return static::$commands[$action['command']];
		}

		if ($action['class'] == 'self') {
			return static::$commands[$action['command']] = 'self';
		}

		$class = $action['class'];
		if (!class_exists($class, false)) {
			include $action['file'];
		}
		if (!class_exists($class, false)) {
			return static::$commands[$action['command']] = false;
		}

		return static::$commands[$action['command']] = $command = new $class();
	}

	public static function process($action) {
		$command = static::command($action['command']);
		$params = array_slice(static::$con->params(), 1);
		$r = null;

		switch (true) {
			case $command == 'self':
				$r = forward_static_call_array('static::'.$action['method'], $params);
				break;
			case ($action['method'] && is_callable(array($command, $action['method']))
				&& count($command->_method($action['method'])->params($required = true)) <= count($params)):
				$command->_setup();
				$r = call_user_func_array(array($command, $action['method']), $params);
				$command->_teardown();
				break;
			case ($action['method'] && is_callable(array($command, $action['method']))):
				con_alert('Incorrect params for {fg:yellow}%s:%s {fg:white}command', $command->name, $action['method']);
				con_eol();
				break;
			case (is_callable($command)
				&& count($command->_method('')->params($required = true)) <= count($params)):
				$command->_setup();
				$r = call_user_func_array($command, $params);
				$command->_teardown();
				break;
			case (is_callable($command)):
				con_alert('Incorrect params for {fg:yellow}%s {fg:white}command', $command->name);
				con_eol();
				break;
		}

		return $r;
	}

	protected static function benchmarks() {
		if (!defined('con_debug') || !con_debug) return;

		// write benchmarking data into output
		con_head('Benchmarks');
	}

	// built-in commands
	protected static function command_ls() {
		return static::command_list();
	}

	protected static function command_list() {
		$commands = static::fetch_commands();

		con_head("Commands");
		foreach ($commands as $commandName) {
			$command = static::command($commandName);
			if ($command == 'self') {
				$list[$commandName] = 'List of available commands';
			} elseif ($command) {
				$list += $command->methods;
			} else {
				con_debug('Unknown command: ' . $commandName);
			}
		}

		con_list($list);
		con_eol();
	}

}
