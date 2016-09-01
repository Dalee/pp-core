#!/usr/bin/env php54
<?php
	//
	// @TODO: refactor to CreateMetaCommand
	// @see PP\Command\HelloCommand
	//

	ini_set('memory_limit', '512M');
	set_time_limit(0);

	define('BASEDIR', realpath(dirname(__FILE__).'/../../'));
	define('DATABASEINI', BASEDIR.'/site/etc/database.ini');
	define('DATATYPEXML', BASEDIR.'/local/etc/datatypes.xml');

	/**
	 *
	 */
	abstract class Database {
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
				print "ERROR: dbname is not set in database.ini\n";
				exit(1);
			}
			$this->dbname = $data['dbname'];
		}
	}

	/**
	 *
	 */
	class PostgreSQL extends Database {
		const TABLEINFO_SQL = "SELECT a.attname, t.typname
			FROM pg_attribute a
			LEFT JOIN pg_type t ON a.atttypid = t.oid
			WHERE
				a.attrelid = (SELECT oid FROM pg_class WHERE relname = ?)";

		protected $pdo = null;
		protected $command = 'psql';

		protected function makeSetClientEncodingQuery() {
			return "SET client_encoding TO '{$this->encoding}'";
		}

		public function __construct($data) {
			parent::__construct($data);

			$dsn = "pgsql:dbname={$this->dbname};host={$this->host};port={$this->port}";
			$this->pdo = new PDO($dsn, $this->user, $this->passwd);
			$this->pdo->exec($this->makeSetClientEncodingQuery());
		}

		public function getFields($tableName) {
			$stmt = $this->pdo->prepare(self::TABLEINFO_SQL);
			$stmt->execute(array($tableName));

			$result = array();
			while(($row = $stmt->fetch(PDO::FETCH_ASSOC))) {
				$result[$row['attname']] = $row['typname'];
			}
			$stmt->closeCursor();
			return $result;
		}

		public function createField($tableName, $fieldName, $fieldType, $fieldNull = 'DEFAULT NULL') {
			$result = $this->pdo->exec(
				sprintf('ALTER TABLE "%s" ADD COLUMN "%s" %s %s', $tableName, $fieldName, $fieldType, $fieldNull)
			);
			if ($result === false) {
				$errorInfo = $this->pdo->errorInfo();
				printf("Error altering table: %s\n", $errorInfo[2]);
				exit(1);
			}
		}
	}

	/**
	 *
	 */
	class DBFactory {
		public static function get() {
			if (!file_exists(DATABASEINI)) {
				printf("Can't find %s, exiting\n", DATABASEINI);
				exit(1);
			}

			$data = parse_ini_file(DATABASEINI, true);
			if(empty($data['database'])) {
				print "No [database] section found\n";
				exit(1);
			}

			$data = $data['database'];
			if(empty($data['dbtype'])) {
				printf("Empty dbtype field, exiting\n");
				return null;
			}

			$obj = null;
			switch($data['dbtype']) {
				case 'pgsql':
					$obj = new PostgreSQL($data);
					break;
				default:
					printf("Database: %s not supported, yet.\n", $data['dbtype']);
					exit(1);
					break;
			}
			return $obj;
		}
	}

	/**
	 *
	 */
	class DatatypeNodeLite {
		protected $name = "";

		public function __construct(/*SimpleXMLElement*/ $node) {
			$baseAttributes = $node->attributes();
			$this->name = strval($baseAttributes->name);
		}

		public function name() {
			return $this->name;
		}
	}

	/**
	 *
	 */
	class Datatype {
		protected $_nodes = array();

		public function __construct() {
			if (!file_exists(DATATYPEXML)) {
				printf("Can't find: %s\n", DATATYPEXML);
				exit(1);
			}
			$this->_parse();
		}

		protected function _parse() {
			$dom = simplexml_load_file(DATATYPEXML);
			$nodeList = $dom->xpath('/model/datatypes/datatype');
			if(empty($nodeList)) {
				printf("No datatypes found, exiting..\n");
				exit(1);
			}

			foreach($nodeList as $domNode) {
				$node = new DatatypeNodeLite($domNode);
				$this->_nodes[$node->name()] = $node;
			}
		}

		/**
		 *
		 */
		public function getNameList() {
			return array_keys($this->_nodes);
		}
	}

	/**
	 *
	 */
	class MetaManager {
		protected $types = null;

		public function __construct() {
			$this->types = new Datatype();
			$this->db = DBFactory::get();
		}

		public function run() {
			// 1. check presence of sys_meta fields in database
			$tableList = $this->types->getNameList();
			foreach($tableList as $tableName) {
				$fieldList = $this->db->getFields($tableName);
				if (!array_key_exists('sys_meta', $fieldList)) {
					$this->db->createField($tableName, 'sys_meta', 'text');
					printf("Created field sys_meta for table %s\n", $tableName);
				}
			}
		}
	}

	$m = new MetaManager();
	$m->run();
