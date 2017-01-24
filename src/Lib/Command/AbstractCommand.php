<?php

namespace PP\Lib\Command;

use PP\Lib\Database\Driver\PostgreSqlDriver;

/**
 * Class AbstractCommand
 * @package PP\Lib\Command
 */
abstract class AbstractCommand extends AbstractBasicCommand {

	/** @var \PXApplication */
	protected $app;

	/** @var \PXDatabase|PostgreSqlDriver */
	protected $db;

	/**
	 * @param \PXApplication $app
	 * @return $this
	 */
	public function setApp(\PXApplication $app) {
		$this->app = $app;

		return $this;
	}

	/**
	 * @param \PXDatabase $db
	 * @return $this
	 */
	public function setDb(\PXDatabase $db) {
		$this->db = $db;

		return $this;
	}

}
