<?php

require_once BASEPATH . '/libpp/plugins/filters/lib/additional_loader.class.inc';

class PXTriggerDatabaseFilters extends PXAbstractDatabaseTrigger {
	function OnBeforeGetDataBuildQuery($db, $format, &$mode, &$selector, &$where, &$order, &$limit) {
		$layout = PXRegistry::getLayout();

		if (!PXRegistry::getApp()->isAdminEngine() || !($layout instanceof PXAdminHTMLLayout)) {
			return;
		}

		$req = PXRegistry::getRequest();

		$filters = PXRegistry::getRequest()->getVar('filters');

		if (empty($filters)) {
			return;
		}

		$layout->setGetVarToSave('filters', $filters);

		if($req->isSetVar('charcheck')) {
			$layout->setGetVarToSave('charcheck', $req->getVar('charcheck'));
		}

		if (empty($filters[$format->id])) {
			return;
		}

		$filters = $filters[$format->id];
		$_where = array();

		foreach ($filters as $name => $value) {
			if (!isset($format->fields[$name]) or !$format->fields[$name]->storageType->filterable()) {
				continue;
			}

			$value = urldecode($value);

			$left  = substr($value, 0, 1) != '^' ? P_LEFT : 0;
			$right = substr($value, -1)   != '$' ? P_RIGHT : 0;

			if (!$left) {
				$value = substr($value, 1);
			}

			if (!$right) {
				$value = substr($value, 0, strlen($value) - 1);
			}

			$_where[] = sprintf("%s.%s %s", $format->id, $name, $db->like($db->escapeString($value), $left | $right));
		}
		
		foreach (PXFiltersPluginAdditionalLoader::getAdditionalFilters($format->id) as $additionalFilterClass) {
			/* @var $additionalFilterClass PXAdditionalFilter */
			$whereAdditionalPart = $additionalFilterClass->getWhere();
			if ($whereAdditionalPart) {
				$_where[] = $whereAdditionalPart;
			}
		}

		if (!empty($_where)) {
			$where = sprintf("%s %s", !empty($where) ? "({$where}) AND " : "", join(' AND ', $_where));
		}
	}
}

?>
