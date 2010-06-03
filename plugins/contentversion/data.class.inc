<?php
class PVersionData {
	private $db;
	private $filter;
	private $app;
	private $format;

	function __construct($format, $filters) {
		$this->db = PXRegistry::getDB();
		$this->app = PXRegistry::getApp();
		$this->filters = $filters;
		$this->format = $format;
	}

	public function getData($page, $onPage, $count, $author = true) {
		$offset = null;
		$limit = null;
		if($count > $onPage) {
			$limit = $onPage;
			$offset = ($page-1) * $onPage;
		}

		$versions = $this->db->getObjectsByWhereLimited($this->app->types[$this->format], null, $this->filters, $limit, $offset, DB_SELECT_TABLE, 'sys_version DESC');
		if(count($versions) && $author) {
			$tmp = $this->db->getObjects($this->app->types['suser'], null);
			foreach($versions as $k => $v) {
				$versions[$k]['sys_modifyer'] = isset($tmp[$v['sys_modifyer']]['title']) ? $tmp[$v['sys_modifyer']]['title'] : '';
			}
		}
		return $versions;
	}

	public function getCount() {
		return $this->db->getObjectsByWhere($this->app->types[$this->format], null, $this->filters, DB_SELECT_COUNT);
	}
}
?>
