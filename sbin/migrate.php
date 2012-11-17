#!/usr/bin/env php5
<?php
	/**
	 * Proxima Portal simple migration manager
	 * 
	 * ./libpp/sbin/migrate.php create hello_world
	 * ./libpp/sbin/migrate.php c hello_world
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
	define('MIGRATEDIR', BASEDIR.'/local/etc/sql');
	define('DATABASEINI', BASEDIR.'/site/etc/database.ini');

	function d2($var) {
		$s = print_r($var, true);
		echo $s."\n";
	}

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

		public function __construct($data) {
			if(!empty($data['host'])) $this->host = $data['host'];
			if(!empty($data['user'])) $this->user = $data['user'];
			if(!empty($data['passwd'])) $this->passwd = $data['passwd'];
			if(!empty($data['port'])) $this->port = $data['port'];
			if(!empty($data['command'])) $this->cli = $data['command'];

			if(empty($data['dbname'])) {
				print "ERROR: dbname is not set in database.ini\n";
				exit(1);
			}
			$this->dbname = $data['dbname'];
		}

		abstract public function setup($filenames);
		abstract public function migrate($filename, $silent);
		abstract public function settedup();
	}

	/**
	 *
	 */
	class PostgreSQL extends Database {
		protected $pdo = null;
		protected $command = 'psql';

		public function __construct($data) {
			parent::__construct($data);

			$dsn = "pgsql:dbname={$this->dbname};host={$this->host};port={$this->port}";
			$this->pdo = new PDO($dsn, $this->user, $this->passwd);
		}

		public function getAll() {
			$stmt = $this->pdo->query(sprintf('SELECT filename FROM %s ORDER BY id DESC', Database::TABLE_NAME));
			$data = $stmt->fetchAll(PDO::FETCH_COLUMN, 0);
			return $data;
		}

		public function settedup() {
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
					print "Failed to create migrations table\n";
					exit(1);
				}
				$stmt->closeCursor();
			}
			
			$stmt_insert = $this->pdo->prepare(sprintf('INSERT INTO %s (filename) VALUES (?)', Database::TABLE_NAME));
			$stmt_check = $this->pdo->prepare(sprintf('SELECT id FROM %s WHERE filename = ?', Database::TABLE_NAME));

			$this->pdo->beginTransaction();
			foreach ($filenames as $_ => $filename) {
				if(!$stmt_check->execute(array($filename))) {
					$this->pdo->rollBack();
					print "Failed to check filename {$filename} in database\n";
					exit(1);
				}
				if($stmt_check->rowCount() == 0) {
					$stmt_check->closeCursor();
					if(!$stmt_insert->execute(array($filename))) {
						$this->pdo->rollBack();
						print "Failed to insert filename: {$filename}\n";
						exit(1);
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

			$migration_data = file_get_contents($src);
			$migration_sql = sprintf(
				"INSERT INTO %s (filename) VALUES (%s);", 
				Database::TABLE_NAME,
				$this->pdo->quote($filename)
			);
			$migration_data .= "\n\n-- append migration info\n{$migration_sql}";
			file_put_contents($dst, $migration_data);

			// execute
			$cmd = array();
			$cmd[] = "{$this->command}";
			$cmd[] = "--no-psqlrc";
			$cmd[] = "--quiet";
			$cmd[] = "--single-transaction";
			$cmd[] = "--no-password";
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
			if(!$silent) {
				putenv("PGOPTIONS=--client-min-messages=warning");	
			}
			system($cmd_joined, $status);
			putenv("PGPASSWORD=");
			putenv("PGOPTIONS=");

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
				case 'mysql': print "Not supported, yet."; exit(1); break;
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
				print "No database.ini file found\n";
				exit(1);
			}

			$data = parse_ini_file(DATABASEINI, true);
			if(empty($data['database'])) {
				print "No [database] section found\n";
				exit(1);
			}

			$this->db = DBFactory::get($data['database']);
			if(is_null($this->db)) {
				print "Failed to get database object\n";
				exit(1);
			}
		}

		/**
		 *
		 */
		protected function help() {
			$help  = "Usage: migrate.php option [argument]\n\n";
			$help .= "Options:\n";
			$help .= "\tc|create <name>\t- create new migration\n";
			$help .= "\ts|setup\t\t- initial setup\n";
			$help .= "\tl|log\t\t- display pending migrations\n";
			$help .= "\tm|migrate\t- perform pending migrations\n";
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
				$this->display("Setup is not performed, please run: setup");
				exit(1);
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
			if($this->argc < 3) {
				$this->display("Migration name absent");
				exit(1);
			}
			if(!$this->db->settedup()) {
				$this->display("Setup is not performed, please run: setup");
			}

			$name = strtolower($this->argv[2]);
			if(!preg_match('/^[a-z_-]+$/', $name)) {
				$this->display("Incorrect name: {$name}. Allowed: a-z, _, and -");
				exit(1);
			}

			$data = "--// Created by: {$this->login}\n\n--sql commands for up goes here\n";
			$name = sprintf("%d_%s", time(), $name);

			$filename = MIGRATEDIR.'/'.$name.'.sql';
			if(file_exists($filename)) {
				$this->display("Strange, but migration allready exists");
				exit(1);
			}

			file_put_contents($filename, $data);

			$filename = str_replace(BASEDIR.'/', './', $filename);
			$this->display("Created: {$filename}");
		}

		/**
		 *
		 */
		protected function getAll() {
			$all = glob(MIGRATEDIR.'/*.sql');
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
				$this->display("Setup is not performed, please run: setup");
				exit(1);
			}

			$applied = $this->db->getAll();
			$available = $this->getAll();

			$pending = array_diff($available, $applied);
			if(empty($pending)) {
				$this->display("No pending migrations");
				exit(0);
			}

			foreach($pending as $_ => $filename) {
				if($this->db->migrate($filename, $this->silent)) {
					$this->display("Successfully migrated: {$filename}");
				} else {
					$this->display("Failed to migrate: {$filename}");
					exit(1);
				}
			}
		}

		/**
		 * FIXME: is this really need?
		 *
		 */
		protected function rollback() {
			if(!$this->db->settedup()) {
				$this->display("Setup is not performed, please run: setup");
				exit(1);
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
	}


	$mgr = new MigrationManager($argc, $argv);
	$mgr->run();
?>
