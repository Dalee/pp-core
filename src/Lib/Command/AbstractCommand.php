<?php

namespace PP\Lib\Command;

use Symfony\Component\Console\Command\Command;

abstract class AbstractCommand extends Command {

	/** @var \PXApplication */
	protected $app;

	/** @var \PXDatabase|\NLPGSQLDatabase */
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
