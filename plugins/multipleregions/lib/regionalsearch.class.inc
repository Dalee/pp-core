<?php

require_once BASEPATH . "/libpp/lib/search.class.inc";

/*
 * ������������ ����� 
 * ��������� pxsearch �������� �  ��������� ������ ����� ��������
 */
class PXRegionalSearch extends PXSearch {
	function beforeSearch($formatid, array &$where) {
		if (PXRegistry::getRequest()->isSetVar('search_by_all_regions')) {
			return;
		}

		$regions = PXMultiRegions::getInstance();

		if ($regions->isRegionalObject($this->app->types[$formatid])) {
			$where[] = $regions->findByRegion();
		}
	}
}

?>