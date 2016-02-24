#!/usr/bin/env php54
<?php
	/**
	 * Proxima Portal simple migration manager
	 *
	 * ./libpp/sbin/migrate.php create hello_world
	 * ./libpp/sbin/migrate.php c hello_world
	 * ./libpp/sbin/migrate.php c -- "ALTER TABLE hello ADD COLUMN world VARCHAR;"
	 * ./libpp/sbin/migrate.php create struct --dry-run
	 *
	 * ./libpp/sbin/migrate.php migrate
	 * ./libpp/sbin/migrate.php m
	 *
	 * ./libpp/sbin/migrate.php setup
	 * ./libpp/sbin/migrate.php s
	 *
	 * ./libpp/sbin/migrate.php log
	 * ./libpp/sbin/migrate.php l
	 *
	 * optional parameter: -s  be silent, only error code notify.
	 */

	ini_set('memory_limit', '512M');
	set_time_limit(0);

	define('BASEDIR', realpath(dirname(__FILE__).'/../../'));
	define('BASEPATH', BASEDIR);
	define('PPLIBPATH', BASEDIR.'/libpp/lib/');
	define('MIGRATEDIR', BASEDIR.'/local/etc/sql');
	define('DATABASEINI', BASEDIR.'/site/etc/database.ini');
	define('DATATYPESXML', BASEDIR.'/local/etc/datatypes.xml');

	// before we include /Debug/functions.inc we need to
	// define IS_WIN constant.
	if (!defined('IS_WIN')) {
		define('IS_WIN', (substr(PHP_OS, 0, 3) == 'WIN'));
	}
	require_once (PPLIBPATH.'/Debug/functions.inc');
	require_once (PPLIBPATH.'/Common/functions.compatibility.inc');

	/**
	 *
	 */
	abstract class Database {
		const TABLE_NAME = '_migrations';

		protected $dbname = '';
		protected $user = 'web';
		protected $host = 'localhost';
		protected $passwd = '';
		protected $port = '5432';
		protected $encoding = 'utf8';

		public function __construct($data) {
			if(!empty($data['host'])) $this->host = $data['host'];
			if(!empty($data['user'])) $this->user = $data['user'];
			if(!empty($data['password'])) $this->passwd = $data['password'];
			if(!empty($data['passwd'])) $this->passwd = $data['passwd'];
			if(!empty($data['port'])) $this->port = $data['port'];
			if(!empty($data['command'])) $this->cli = $data['command'];
			if(!empty($data['encoding'])) $this->encoding = $data['encoding'];

			if(empty($data['dbname'])) {
				$this->fatal("dbname is not set in database.ini");
			}
			$this->dbname = $data['dbname'];
		}

		abstract public function setup($filenames);
		abstract public function migrate($filename, $silent);
		abstract public function settedUp();

		protected function fatal($message, $code = 1) {
			print ('ERROR: '.$message."\n");
			exit((int)$code);
		}
	}

	/**
	 *
	 */
	class PostgreSQL extends Database {
		protected $pdo = null;
		protected $command = 'psql';

		protected function makeSetClientEncodingQuery() {
			return "SET client_encoding TO '{$this->encoding}'";
		}

		public function __construct($data) {
			parent::__construct($data);

			$dsn = "pgsql:dbname={$this->dbname};host={$this->host};port={$this->port}";
			$this->pdo = new PDO($dsn, $this->user, $this->passwd);

			// set right encoding
			$this->pdo->exec($this->makeSetClientEncodingQuery());
		}

		public function getAll() {
			$stmt = $this->pdo->query(sprintf('SELECT filename FROM %s ORDER BY id DESC', Database::TABLE_NAME));
			$data = $stmt->fetchAll(PDO::FETCH_COLUMN, 0);
			return $data;
		}

		public function settedUp() {
			$stmt = $this->pdo->prepare(sprintf('select * from pg_catalog.pg_tables WHERE tablename = ?'));
			$stmt->execute(array(Database::TABLE_NAME));
			$rowCount = $stmt->rowCount();
			$stmt->closeCursor();
			return ($rowCount > 0);
		}

		/**
		 *
		 */
		public function setup($filenames) {
			if(!$this->settedup()) {
				$stmt = $this->pdo->prepare(
					sprintf(
						'create table %s ( id serial primary key, filename varchar(255) ) with oids',
						Database::TABLE_NAME)
				);
				if(!$stmt->execute()) {
					$this->fatal("Failed to create migrations table");
				}
				$stmt->closeCursor();
			}

			$stmt_insert = $this->pdo->prepare(sprintf('INSERT INTO %s (filename) VALUES (?)', Database::TABLE_NAME));
			$stmt_check = $this->pdo->prepare(sprintf('SELECT id FROM %s WHERE filename = ?', Database::TABLE_NAME));

			$this->pdo->beginTransaction();
			foreach ($filenames as $_ => $filename) {
				if(!$stmt_check->execute(array($filename))) {
					$this->pdo->rollBack();
					$this->fatal("Failed to check filename {$filename} in database", 1);
				}
				if($stmt_check->rowCount() == 0) {
					$stmt_check->closeCursor();
					if(!$stmt_insert->execute(array($filename))) {
						$this->pdo->rollBack();
						$this->fatal("Failed to insert file: {$filename}.\nDB Error: " . json_encode($this->pdo->errorInfo()), 1);
					}
					print "Marked {$filename} as already applied\n";
					$stmt_insert->closeCursor();
				}
			}
			$this->pdo->commit();
		}

		public function migrate($filename, $silent) {
			// do not use PDO here to perform migration and mark it as done in one transaction
			$src = MIGRATEDIR . '/' . $filename;
			$dst = tempnam(BASEDIR.'/tmp', 'sql_');

			$migration_sql = sprintf(
				"INSERT INTO %s (filename) VALUES (%s);",
				Database::TABLE_NAME,
				$this->pdo->quote($filename)
			);

			// creating migration
			$migration_data = "-- set right client encoding\n";
			$migration_data .= $this->makeSetClientEncodingQuery() . ";";

			$migration_data .= "\n\n-- migration data\n";
			$migration_data .= file_get_contents($src) . ";";

			$migration_data .= "\n\n-- append migration info\n";
			$migration_data .= $migration_sql . ";";

			file_put_contents($dst, $migration_data);

			// execute
			$cmd = array();
			$cmd[] = "{$this->command}";
			$cmd[] = "-X"; // --no-psqlrc
			$cmd[] = "-q"; // --quiet
			$cmd[] = "-1"; // --single-transaction
			//$cmd[] = "--no-password"; not supported in 8.3
			$cmd[] = "-v ON_ERROR_STOP=1";
			$cmd[] = sprintf("--host=%s", escapeshellarg($this->host));
			$cmd[] = sprintf("--port=%s", escapeshellarg($this->port));
			$cmd[] = sprintf("--username=%s", escapeshellarg($this->user));
			$cmd[] = sprintf("--dbname=%s", escapeshellarg($this->dbname));
			$cmd[] = sprintf("--file=%s", escapeshellarg($dst));
			if($silent) {
				$cmd[] = "> /dev/null 2>&1";
			}
			$cmd_joined = implode(" ", $cmd);

			putenv("PGPASSWORD={$this->passwd}");
			system($cmd_joined, $status);
			putenv("PGPASSWORD=");

			unlink($dst);
			$status = intval($status);
			return $status === 0;
		}
	}


	/**
	 *
	 */
	class DBFactory {
		public static function get($data) {
			if(empty($data['dbtype'])) {
				return null;
			}

			$obj = null;
			switch($data['dbtype']) {
				case 'pgsql': $obj = new PostgreSQL($data); break;
				case 'mysql': print("Not supported, yet.\n"); exit(1); break;
			}
			return $obj;
		}
	}


	/**
	 *
	 */
	class MigrationManager {
		protected $argv = null;
		protected $argc = null;
		protected $db = null;
		protected $silent = false;

		public function __construct($argc, $argv) {
			$this->argc = $argc;
			$this->argv = $argv;
			$this->login = $_SERVER['USER'];
		}

		public function run() {
			// cut-out -s param if exists
			foreach($this->argv as $idx => $param) {
				if(!strcmp('-s', $param)) {
					unset($this->argv[$idx]);
					$this->silent = true;
				}
			}

			$this->connect();
			$this->argc = count($this->argv);
			if($this->argc < 2) {
				$this->help();
				return;
			}

			$action = strtolower($this->argv[1]);
			switch ($action) {
				case 'l':
				case 'log':
					$this->log();
					break;

				case 'c':
				case 'create':
					$this->create();
					break;

				case 's':
				case 'setup':
					$this->setup();
					break;

				case 'm':
				case 'migrate':
					$this->migrate();
					break;

				case 'e':
				case 'execute':
					$this->execute();
					break;

				case 'r':
				case 'rollback':
					$this->rollback();
					break;

				default:
					$this->help();
					break;
			}
		}

		/**
		 *
		 */
		protected function connect() {
			if(!file_exists(DATABASEINI)) {
				$this->fatal("No database.ini file found", 1);
			}

			$data = parse_ini_file(DATABASEINI, true);
			if(empty($data['database'])) {
				$this->fatal("No [database] section found", 1);
			}

			$this->db = DBFactory::get($data['database']);
			if(is_null($this->db)) {
				$this->fatal("Failed to get database object", 1);
			}
		}

		/**
		 *
		 */
		protected function help() {
			$help  = "Usage: migrate.php option [argument]\n\n";
			$help .= "Options:\n";
			$help .= "\tc|create <name> [<query>]\t- create new migration\n";
			$help .= "\tc|create -- <query>\n";
			$help .= "\ts|setup\t\t- initial setup\n";
			$help .= "\tl|log\t\t- display pending migrations\n";
			$help .= "\tm|migrate\t- perform pending migrations\n";
			$help .= "\te|execute <name>\t\t- execute some migration\n";
			$this->display($help);

			if(!$this->db->settedup()) {
				$this->display("Setup is not performed, please run: setup");
			}
		}

		/**
		 *
		 */
		protected function log() {
			if(!$this->db->settedup()) {
				$this->fatal("Setup is not performed, please run: setup", 1);
			}

			$applied = $this->db->getAll();
			$all = $this->getAll();
			$pending = array_diff($all, $applied);

			print "Pending migrations:\n";
			if(count($pending) == 0) {
				$this->display("\tNo pending migrations found");
			} else {
				foreach($pending as $id => $filename) {
					$this->display("\t{$filename}");
				}
			}
		}

		/**
		 * Just create new migration
		 *
		 */
		protected function create() {
			if (!$this->db->settedup()) {
				$this->fatal("Setup is not performed, please run: setup");
			}

			// fetch flags if exists
			if ($dryrun = array_search('--dry-run', $this->argv)) {
				unset($this->argv[$dryrun]);
				$dryrun = true;
			}
			$this->argv = array_values($this->argv);
			$this->argc = count($this->argv);

			// go on
			if ($this->argc < 3) {
				$this->fatal("Migration name absent", 5);
			}
			// special behavior of -- name
			if ($this->argv[2] == '--') {
				$first_query_string = trim($this->argv[3], " \t\n\r;");
				if (empty($first_query_string)) {
					$this->fatal("Special '--' filename requires query argument", 3);
				}
				list($query) = explode(';', $first_query_string);
				$this->argv[2] = preg_replace('/[^a-z0-9\-]+/', '_', strtolower($query));

			} else if ($this->argc === 3) {
				$this->argv[3] = $this->_sqlCreateTableByMigration($this->argv[2]);
				$this->argv[2] = 'create_table_'.preg_replace('/[^a-z0-9\-]+/', '_', $this->argv[2]);
				$this->argc = 4;
			}

			$name = strtolower($this->argv[2]);
			if(!preg_match('/^[a-z0-9_-]+$/', $name)) {
				$this->fatal("Incorrect name: {$name}. Allowed: a-z, 0-9, _, and -");
			}

			$data = "-- // Created by: {$this->login}\n\n";
			$name = sprintf("%s_%s", date('YmdHis'), $name);

			// put command line sql data
			if (!empty($this->argv[3])) {
				$data .= join(PHP_EOL, array_slice($this->argv, 3));
			} else {
				$data .= "-- sql commands for up goes here";
			}
			$data .= PHP_EOL;

			$filename = MIGRATEDIR.'/'.$name.'.sql';
			if (file_exists($filename)) {
				$this->fatal("Strange, but migration already exists", 1);
			}

			$dryrun && printf("Storing file %s:%s%s", $filename, PHP_EOL, $data);
			$dryrun || file_put_contents($filename, $data);

			$filename = str_replace(BASEDIR.'/', './', $filename);
			$this->display("Created: {$filename}");
		}

		/**
		 *
		 */
		protected function getAll($migration = '*.sql') {
			$all = glob(MIGRATEDIR.'/'.$migration);
			foreach($all as $idx => $fullname) {
				$all[$idx] = basename($fullname);
			}
			sort($all);
			return $all;
		}

		/**
		 *
		 */
		protected function setup() {
			$all = $this->getAll();
			$this->db->setup($all);
			$this->display("Setup finished successfuly");
		}

		/**
		 *
		 */
		protected function migrate() {
			if(!$this->db->settedup()) {
				$this->fatal("Setup is not performed, please run: setup");
			}

			$applied = $this->db->getAll();
			$available = $this->getAll();

			$pending = array_diff($available, $applied);
			if(empty($pending)) {
				$this->fatal("No pending migrations", 0);
			}

			foreach($pending as $_ => $filename) {
				if($this->db->migrate($filename, $this->silent)) {
					$this->display("Successfully migrated: {$filename}");
				} else {
					$this->fatal("Failed to migrate: {$filename}", 1);
				}
			}
		}

		protected function execute() {
			if ($this->argc < 3) {
				$this->fatal("Migration name absent", 1);
			}

			$applied = $this->db->getAll();
			$all = $this->getAll();
			$pending = array_diff($all, $applied);
			$wildcard = preg_replace('@^(\./)?local/etc/sql/@', '', strtolower($this->argv[2]));

			$lastcreated = ($wildcard === '-');
			$lastcreated && ($wildcard = reset($pending));

			if (!preg_match('/^[a-z0-9_\-\?\*]+(\.sql)?$/', $wildcard)) {
				$this->fatal("Incorrect name: {$wildcard}. Allowed: a-z, 0-9, _, and -, and wildcards", 1);
			}
			if (!$this->db->settedup()) {
				$this->fatal("Setup is not performed, please run: setup", 2);
			}

			if (strpos($wildcard, '*') === false) {
				$wildcard = '*' . $wildcard . '*';
			}
			if (strpos($wildcard, '.sql') === false) {
				$wildcard .= '.sql';
			}
			$available = $this->getAll($wildcard);
			if (empty($available)) {
				$this->fatal("Migration '{$wildcard}' was not found.", 3);
			}
			$filename = reset($available);

			if (in_array($filename, $applied)) {
				$answer = $this->prompt("Migration {$filename} marked as imported.\nDo you really want to continue? [y/N]");
				if (strtolower($answer[0]) !== 'y') {
					$this->display("Execution cancelled");
					return;
				}
			} elseif ($lastcreated) {
				$answer = $this->prompt("Do you want to migrate {$filename}? [y/N]");
				if (strtolower($answer[0]) !== 'y') {
					$this->display("Execution cancelled");
					return;
				}
			}
			if ($filename) {
				if ($this->db->migrate($filename, $this->silent)) {
					$this->display("Successfully migrated: {$filename}");
				} else {
					$this->fatal("Failed to migrate: {$filename}", 4);
				}
			}
		}

		/**
		 * FIXME: is this really need?
		 *
		 */
		protected function rollback() {
			if(!$this->db->settedup()) {
				$this->fatal("Setup is not performed, please run: setup", 1);
			}
		}

		/**
		 *
		 */
		protected function display($message) {
			if(!$this->silent) {
				print($message."\n");
			}
		}
		protected function fatal($message, $code = 1) {
			if (!$this->silent) {
				print ('ERROR: '.$message."\n");
			}
			exit((int)$code);
		}

		protected function prompt($message) {
			if (!$this->silent) {
				echo $message . ' ';
				return `read __tmp && echo \$__tmp`;
			}
			return false;
		}

		protected function _sqlCreateTableByMigration($name) {
			require_once (BASEDIR . '/libpp/lib/XML/classes.inc');
			require_once (BASEDIR . '/libpp/lib/StorageType/classes.inc');
			foreach (glob(BASEDIR . '/local/lib/StorageType/*.class.inc') as $stf) {
				require_once ($stf);
			}
			foreach (glob(BASEDIR . '/libpp/plugins/*/storageTypes/*.class.inc') as $stf) {
				require_once ($stf);
			}
			foreach (glob(BASEDIR . '/local/plugins/*/storageTypes/*.class.inc') as $stf) {
				require_once ($stf);
			}


			$sys_fields = array(
				'sys_order' => 'INTEGER',
				'sys_owner' => 'INTEGER DEFAULT NULL REFERENCES suser ON DELETE SET NULL ON UPDATE CASCADE',
				'sys_created' => 'TIMESTAMP DEFAULT now()',
				'sys_modified' => 'TIMESTAMP DEFAULT now()',
				'sys_meta' => 'VARCHAR',
			);

			// try to collect fields data by datatype or just use defaults
			$xml = PXml::load(DATATYPESXML);
			@list ($datatype) = $xml->xpath('/model/datatypes/datatype[@name="'.$name.'"]');
			if (empty($datatype)) {
				$this->fatal("Unknown datatype name ".$name);
			}
			if ($datatype && $datatype->name == $name) {
				// $this->fatal('Datatype '.addslashes($name).' not exists');
				$fields = $this->_makeFieldsByDatatype($datatype);
			} else {
				$fields = array(
					'id' => 'SERIAL PRIMARY KEY',
					'title' => 'VARCHAR',
				);
			}

			$fields += $sys_fields;

			// need to call trigger here: onMigrateCreateTable or something instead of hack
			if (isset($fields['sys_regions'])) {
				$fields['sys_reflex_id'] = 'INTEGER';
			}

			foreach ($fields as $key => $field) {
				$fields[$key] = sprintf('  %-16s %s', $key, $field);
			}

			$statement = sprintf('CREATE TABLE %s (%s%s%2$s) WITH OIDS;', $datatype->name, PHP_EOL, join(",".PHP_EOL, $fields));

			return $statement;
		}

		protected function _makeFieldsByDatatype($datatype) {
			$fields = array();

			// load storagetypes here to determine their datatypes
			$attributes = array_merge((array)$datatype->xpath('attribute|group/attribute'));
			foreach ($attributes as $att) {
				list ($storageType) = explode('|', $att->storagetype);
				$class = PXStorageType::getClassByName($storageType);
				if ($class == 'PXStorageType') {
					$this->fatal("Unexpected storage type: {$storageType}", 9);
				}
				try {
					if (!call_user_func(array($class, 'storedInDb'))) continue;
				} catch (Exception $e) {
					// dummy
				}
				$sqltype = constant("{$class}::defaultSQLType");
				$sqltype || ($sqltype = 'VARCHAR');
				$fields[$att->name] = $sqltype;
			}

			// fix parent
			if ($datatype->parent) {
				$fields['parent'] = isset($fields['parent'])? $fields['parent'] : 'INTEGER';
				$fields['parent'] .= ' DEFAULT NULL REFERENCES '.($datatype->parent).' ON DELETE CASCADE ON UPDATE CASCADE';
			}

			return $fields;
		}
	}


	$mgr = new MigrationManager($argc, $argv);
	$mgr->run();
