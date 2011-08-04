<?php

abstract class PXAdditionalFilter {

	protected $app;
	/**
	 * @var PXRequest
	 */
	protected $request;
	/**
	 * @var PXDatabase
	 */
	protected $db;
	protected $filterName = 'language';
	protected $formatId;
	protected $filterValue = null;

	public function __construct($formatId) {
		$this->formatId = $formatId;
		$this->app = PXRegistry::getApp();
		$this->request = PXRegistry::getRequest();
		$this->db = PXRegistry::getDb();

		$filtersParam = $this->request->getVar('filters');
		if (is_array($filtersParam) && isset($filtersParam[$formatId][$this->filterName])) {
			$this->filterValue = (string) $filtersParam[$formatId][$this->filterName];
		}
	}

	abstract public function html();

	abstract public function getWhere();

	public function getName() {
		return $this->filterName;
	}

}

?>
