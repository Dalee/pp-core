<?php
/**
 * test command
 *
 * @category	Commands
 * @author  	Alex Yaroshevich <zxqfox@gmail.com>
 */

// namespace commands;

require_once PPLIBPATH . 'Common/functions.test.inc';

/**
 * Unit testing tool
 */
class PXCommandTest extends PXAbstractCommand {

	const TEST_EXTENSION = 'test.inc';
	const SQL_EXTENSION  = 'test.sql';
	const PSQL_EXTENSION = 'test.sql.inc';

	protected $name        = 'test';
	protected $help        = <<<'help'
Pure test units based on callbacks.

// t($name, $callback, $expected);
// tt($classname);
t(
	// title for that test
	'simple test units example',
	// routine
	function() {
		// here we returning test result
		return array(15, true);
	},
	// tester callback or static expected result
	function($result) {
		return is_array($result) && $result[0] === 15 && $result[1] === true;
	} // or just array(15, true)
);
// will result in
  1. simple test units example                0.0010ms    [  OK  ]

Also tt function will search for doc tests in static methods and execute them one by one.
tt('someclass');

class someclass {
	/**
	 * test args >>> result --- both in json
	 * @test [false] >>> "false"
	 * @test [true] >>> "true"
	 * @test [false, "wowkee"] >>> "true"
	 */
	public static function bool($v, $d = 'false') {
		return $v ? 'true' : $d;
	}
...
// will result in something like that
 a. someclass::bool
   2. [false]              >>> "false":                               0.0010ms    [  OK  ]
   3. [true]               >>> "true":                                0.0010ms    [  OK  ]
   3. [false, "wowkee"]    >>> "wowkee":                              0.0011ms    [  OK  ]

Look also at working examples inside ~*/tests/ directory.

Also if fails happend command returns code 1 so you can use it for testing before deploy:
./libpp/sbin/pp.php test:all -q && sudo deploy-command

help;

	protected $flags = array(
		'help' => '?',
		'quite' => 'q',
		'verbose' => 'v',
		'bail' => 'b',
		'diff-sidebyside' => 'y',
		'diff-uniform' => 'u',
	);

	public static $quite = false;
	public static $verbose = false;
	public static $bail = false;
	public static $diffmode = false;

	protected static $fails = array();

	public static function _fail($data) {
		self::$fails[] = $data;
		self::$bail && self::done();
	}

	// it's dirty
	protected static function countTests($path) {
		$content = file_get_contents($path);
		preg_match_all('@\bt\s*\(@', $content, $m);
		$count = count($m[0]);

		preg_match_all('@\btt\s*\(\s*([^\)]+)@', $content, $ttAims);
		foreach ($ttAims[1] as $ttAim) {
			$aim = null;
			@eval("\$aim = [$ttAim];");
			if (!$aim) continue;
			$file = $aim[0];
			if (!file_exists($file)) {
				echo "Aim \"{$file}\" not found\n";
				continue;
			}
			preg_match_all('#\s*@test\s*#', file_get_contents($file), $m);
			$count += count($m[0]);
		}
		return $count;
	}

	/**
	 * Run test on unit(s))
	 */
	public function __invoke($unit = null) {
		$flags = PXConsole::flags() + array_fill_keys(array_keys($this->flags), 0);

		if ($flags['help']) {
			$this->help();
			return;
		}

		self::$quite = $flags['quite'];
		self::$verbose = $flags['verbose'];
		self::$bail = $flags['bail'];
		self::$diffmode = $flags['diff-sidebyside'] ? 'sidebyside' : ($flags['diff-uniform'] ? 'uniform' : false);

		$units = array();
		foreach (pp_base_paths('tests', 1) as $plugin => $path) {

			// recursive fetch tests
			$tests = array(); //glob($path . '*.' . self::TEST_EXTENSION);
			while ($d = glob($path . '*', GLOB_ONLYDIR)) {
				$path .= '*';
				$tests = array_merge($tests, glob($path . '.' . self::TEST_EXTENSION));
				$path .= '*/';
			}

			foreach ($tests as $testPath) {
				$unit_basepath = str_replace('.' . self::TEST_EXTENSION, '', $testPath);
				$unit_sqlpath = file_exists($unit_sqlpath = $unit_basepath . '.' . self::SQL_EXTENSION)? $unit_sqlpath : null;
				$unit_psqlpath = file_exists($unit_psqlpath = $unit_basepath . '.' . self::PSQL_EXTENSION)? $unit_sqlpath : null;
				$unit_item = array(
					'name' => str_replace('.' . self::TEST_EXTENSION, '', pathinfo($testPath, PATHINFO_BASENAME)),
					'plugin' => $plugin,
					'path' => $testPath,
					'sqlpath' => $unit_sqlpath,
					'psqlpath' => $unit_psqlpath,
				);
				$units[] = $unit_item;
			}
		}

		usort($units, function ($a, $b) {
			$q = strcmp($a['plugin'], $b['plugin']);
			if ($q !== 0) return $q;
			$q = strcmp($a['name'], $b['name']);
			return $q;
		});

		if (self::$quite) {
			(self::$quite < 2) ? ob_start(function(){}) : ob_start(function(){}, 0, false);
			error_reporting(0);
		}

		switch (true) {
			case empty($unit):
				static::show_units($units);
				break;

			case in_array($unit, $units):
				static::run_unit($unit, $unit['path']);
				static::done();
				break;

			default:
				$unitpat = $unit;
				strpos($unitpat, '*') !== false && $unitpat = str_replace('*', '.*', $unitpat);
				strpos($unitpat, $unitpat[0], 1) === false && $unitpat = '@^'.$unitpat.'@i';

				$accepted = array();
				foreach ($units as $k => $u) {
					(1 === @preg_match($unitpat, $u['name'])) && $accepted[$k] = $u;
				}

				if (empty($accepted)) {
					con_alert('There is no test units with pattern "'.addslashes($unit).'"');
					die(2);
				}

				static::run_units($accepted);
				static::done();
		}
	}

	protected static function show_units($units) {
		con_head('Testunits');
		$plug = null;
		foreach ($units as $k => $unit) {
			if ($unit['plugin'] !== $plug) {
				con_subhead($plug = $unit['plugin']);
			}
			$file = $unit['path'];
			$count = static::countTests($file);
			$count = $count . ' test' . ($count === 1? ' ' : 's');
			$relpath = './' . str_replace(BASEPATH.'/', '', $file);
			con_printf('{fg:yellow}'.\_tt_letter($k, 2).'. {fg:white}%-25s  %10s{end}    %s{nl}', $unit['name'], $count, $relpath);
		}
		con_printf('{nl}');
	}


	protected static function run_units($units) {
		con_head('Testunits');
		$plug = null;
		foreach ($units as $k => $unit) {
			static::run_unit($unit, $unit['path']);
			continue;
			if ($unit['plugin'] !== $plug) {
				con_subhead($unit['plugin']);
			}
			$file = $unit['path'];
			$count = $count . ' test' . ($count === 1? ' ' : 's');
			$relpath = './' . str_replace(BASEPATH.'/', '', $file);
			con_printf('{fg:yellow}'.\_tt_letter($k, 2).'. {fg:white}%-25s  %10s{end}    %s{nl}', $unit['name'], $count, $relpath);
		}
		con_printf('{nl}');
	}

	protected static function run_unit($unit, $path = null) {
		con_printf('Unit "{fg:white}%s{end}" %s{nl}', $unit['name'], $unit['plugin']? "({fg:yellow}{$unit['plugin']}{end})" : '');

		if (@$unit['sqlpath'] && file_exists(@$unit['sqlpath'])) {
			static::db_env(file_get_contents($unit['sqlpath']));
		}
		if (@$unit['psqlpath'] && file_exists(@$unit['sqlpath'])) {
			ob_start();
			include($unit['psqlpath']);
			$sql = ob_get_clean();
			static::db_env($sql);
		}

		$path || ($path = is_array($unit)? $unit['path'] :
				pp_fetch_file(pp_base_paths('tests', 1), $unit['name'], self::TEST_EXTENSION));

		$db = static::db_env();
		$app = PXRegistry::getApp();

		include $path;

		static::db_env_clean();
	}

	protected static $db_context = false;
	protected static function db_env($sql = null) {
		if (!self::$db_context) {
			self::$db_context = PXRegistry::getDB();
			self::$db_context->cacheOff();
			self::$db_context->transactionBegin();
		}

		if ($sql) {
			self::$db_context->query($sql, true);
		}

		return self::$db_context;
	}

	protected static function db_env_clean() {
		self::$db_context && self::$db_context->transactionRollback();
		self::$db_context = false;
	}

	protected static function done() {
		if (self::$quite) {
			while (ob_end_clean()) ;
		}
		if (empty(self::$fails)) {
			con_printf('{fg:lgreen}done.{end}{nl}');
			die(0);
		}
		con_printf('{fg:lred}done with %d fails{end}{nl}', count(self::$fails));
		die(1);
	}

	/**
	 * Execute all tests
	 */
	public function all() {
		$this->__invoke('*');
	}

	/**
	 * Help
	 */
	public function help() {
		return parent::__invoke();
	}
}
