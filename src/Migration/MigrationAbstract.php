<?php

namespace PP\Migration;

/**
 * Base class for all migrations in `app/migrations`
 *
 * Class MigrationAbstract
 * @package PP\Migration
 */
abstract class MigrationAbstract {

	/** @var string[] */
	private $sql;

	/**
	 * @internal
	 */
	final public function __construct() {
		$this->sql = [];
	}

	/**
	 * Use this method to write migrations
	 *
	 * @return void
	 */
	abstract public function up();

	/**
	 *
	 * @param string $rawSql
	 * @return $this
	 */
	final protected function addSql($rawSql) {
		$this->sql[] = $rawSql;
		return $this;
	}

	/**
	 * @return string[]
	 * @internal
	 */
	final public function getSqlList() {
		return $this->sql;
	}
}
