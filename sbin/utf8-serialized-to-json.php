#!/usr/bin/env php54
<?php

	ini_set('memory_limit', '512M');
	set_time_limit(0);

	define('BASEDIR', realpath(dirname(__FILE__).'/../../'));
	define('DATABASEINI', BASEDIR.'/site/etc/database.ini');
	define('DATATYPEXML', BASEDIR.'/local/etc/datatypes.xml');

	function d2($var) {
		$var = (is_bool($var)) ? ($var === true ? 'TRUE' : 'FALSE') : $var;
		$s = print_r($var, true);
		echo $s."\n";
	}

	function __json_encode_koi_k2u(&$value) {
		$value = iconv('koi8-r', 'utf-8', $value);
	}

	function json_encode_koi($value, $options = 0) {
		if (empty($value)) {
			// dummy
		} elseif (is_scalar($value)) {
			__json_encode_koi_k2u($value);
		} else {
			array_walk_recursive($value, '__json_encode_koi_k2u');
		}

		switch(PHP_MINOR_VERSION) {
			case 2:
				$value = json_encode($value);
				break;
			case 3:
			case 4:
				$value = json_encode($value, $options);
				break;
		}

		return $value;
	}

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
		protected $limit = 1000;

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

		public function getCurrentLimit() {
			return $this->limit;
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

		public function copyField($tableName, $srcField, $dstField) {
			$this->pdo->beginTransaction();
			$query = sprintf('UPDATE %s SET %s = %s', $tableName, $dstField, $srcField);
			$this->pdo->exec($query);
			$this->pdo->commit();
		}

		public function getRowsLimited($tableName, $fieldList, $offset) {
			$query = sprintf('SELECT %s FROM %s ORDER BY id ASC LIMIT ? OFFSET ?',  implode(', ', $fieldList), $tableName);
			if (! ($stmt = $this->pdo->prepare($query))) {
				d2($this->pdo->errorInfo());
				exit();
			}
			if (! ($stmt->execute(array($this->limit, $offset)))) {
				d2($this->pdo->errorInfo());
				exit();
			}

			$result = array();
			while(($row = $stmt->fetch(PDO::FETCH_ASSOC))) {
				$result[] = $row;
			}

			$stmt->closeCursor();
			return $result;
		}

		public function bulkUpdate($tableName, $fieldName, $valueList) {
			$query = sprintf('UPDATE %s SET %s = ? WHERE id = ?', $tableName, $fieldName);
			if (! ($stmt = $this->pdo->prepare($query))) {
				d2($this->pdo->errorInfo());
				exit();
			}
			foreach ($valueList as $_ => $row) {
				if (! ($stmt->execute(array($row['value'], $row['id'])))) {
					d2($this->pdo->errorInfo());
					exit();
				}
			}
			$stmt->closeCursor();
		}

		public function createField($tableName, $fieldName, $fieldType, $fieldNull = 'DEFAULT NULL') {
			$this->pdo->beginTransaction();
			$result = $this->pdo->exec(
				sprintf('ALTER TABLE "%s" ADD COLUMN "%s" %s %s', $tableName, $fieldName, $fieldType, $fieldNull)
			);
			if ($result === false) {
				$errorInfo = $this->pdo->errorInfo();
				$this->pdo->rollBack();
				printf("Error altering table: %s\n", $errorInfo[2]);
				exit(1);
			}
			$this->pdo->commit();
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
		protected $attributes = array();

		public function __construct(/*SimpleXMLElement*/ $node) {
			$baseAttributes = $node->attributes();
			$this->name = strval($baseAttributes->name);

			// hasListOfAttributes?
			$nodeList = $node->xpath('attribute');
			foreach ($nodeList as $_ => $nodeAttribute) {
				$attrs = $nodeAttribute->attributes();
				$this->attributes[] = new DatatypeAttribute($attrs);
			}
			$nodeList = $node->xpath('group/attribute');
			foreach ($nodeList as $_ => $nodeAttribute) {
				$attrs = $nodeAttribute->attributes();
				$this->attributes[] = new DatatypeAttribute($attrs);
			}
		}

		public function getName() {
			return $this->name;
		}

		public function getAttributes() {
			return $this->attributes;
		}
	}

	class DatatypeAttribute {
		protected $name = "";
		protected $displaytype = null;
		protected $storagetype = null;

		public function __construct($data) {
			$this->name = strtolower(strval($data->name));
			$this->displaytype = strtolower(strval($data->displaytype));
			$this->storagetype = strtolower(strval($data->storagetype));
		}

		public function getStorageType() {
			return $this->storagetype;
		}

		public function getName() {
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
				$this->_nodes[$node->getName()] = $node;
			}
		}

		/**
		 *
		 */
		public function getNameList() {
			return array_keys($this->_nodes);
		}

		public function getNodeList() {
			return $this->_nodes;
		}
	}



	class SerializeJsonUtf8PrepareUpdater {
		protected $poiFields = array('serialized');
		protected $skipTables = array('feedback_mail');

		public function __construct() {
			$this->datatype = new Datatype();
			$this->db = DBFactory::get();
		}

		protected function createFieldsAndAnalyze() {
			$datatypes = $this->datatype->getNodeList();
			$updateTableFieldList = array();

			// analyze and create mandatory fields
			foreach ($datatypes as $_ => $datatype) {
				$tableName  = $datatype->getName();
				$attributes = $datatype->getAttributes();
				$fieldList = $this->db->getFields($tableName);
				
				foreach ($attributes as $_ => $attribute) {
					$storagetype = $attribute->getStorageType();
					if (! in_array($storagetype, $this->poiFields)) {
						continue;
					}

					$backupField = (in_array($backupField, $fieldList)) ? 
						null : 
						sprintf('%s__backup', $attribute->getName());

					$updateTableFieldList[] = array(
						$tableName,
						$attribute->getName(),
						$backupField
					);
				}
			}

			return $updateTableFieldList;
		}

		protected function convertBulk($tableName, $fieldName, $backupField) {
			if (in_array($tableName, $this->skipTables)) {
				printf("[SKIP] - {$tableName}\n");
				return;
			}

			printf("{$tableName} ");
			$offset = 0;
			$processed = 0;
			while (1) {
				$rowList = $this->db->getRowsLimited($tableName, array('id', $fieldName), $offset);
				if (empty($rowList)) {
					break;
				}
				$updatedRowList = array();
				foreach ($rowList as $_ => $row) {
					$sign = substr($row[$fieldName], 0, 2);
					if ( $sign === 'a:') {
						$updatedRowList[] = array(
							'id' => $row['id'],
							'value' => json_encode_koi(unserialize($row[$fieldName]))
						);
						$processed += 1;
					}
				}

				$this->db->bulkUpdate($tableName, $fieldName, $updatedRowList);
				$offset += $this->db->getCurrentLimit();
			}
			printf("- [OK], processed = %d\n", $processed);
		}

		public function run() {
			$updateTableFieldList = $this->createFieldsAndAnalyze();
			foreach ($updateTableFieldList as $_ => $meta) {
				if (! is_null($meta[2])) {
					printf("backing up {$tableName}");
					$this->db->createField($meta[0], $meta[2], 'text');
					$this->db->copyField($meta[0], $meta[1], $meta[2]);
				}
				$this->convertBulk($meta[0], $meta[1], $meta[2]);
			}
		}
	}

	$processor = new SerializeJsonUtf8PrepareUpdater();
	$processor->run();
