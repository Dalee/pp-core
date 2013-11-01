<?php

/**
 * Deferred objects publication
 * Отложенная публикация объектов
 */

class PXCronRunDeferredStatus extends PXAbstractCronRun {

	function __construct() {
		$this->name = 'Отложенная публикация объектов';
	}

	function Run(&$app, &$db, &$tree, $matchedTime, $matchedRule) {

		$this->plugin = $app->plugins['deferredstatus'];

		$deferredDatatypes = isset($this->plugin->params['datatypes'])? $this->plugin->params['datatypes'] : '';
		$deferredDatatypes = array_filter(explode(',', $deferredDatatypes));
		if (empty($deferredDatatypes)) {
			Label('Plugin requires param "datatypes" with colon-delimited list of deferred datatypes');
			exit;
		}

		Label('Starting...');
		$db->cacheOff();

		$shown = $hidden = array();
		$dropCache = false;
		foreach ($deferredDatatypes as $type) {

			$format = $app->types[$type];

			// fetch unpublished
			$activeStates = join(',', PXDeferredStatusHelper::statesIds('prepared', 'published'));
			$objects = $db->getObjectsByWhere($format, NULL, <<<SQL_WHERE
				(sys_deferred_state IN ($activeStates)) AND (
					(sys_deferred_since IS NOT NULL AND NOT status AND (NOW() BETWEEN sys_deferred_since AND COALESCE(sys_deferred_until, NOW()))) OR
					(sys_deferred_until IS NOT NULL AND status AND NOW() >= sys_deferred_until)
				)
SQL_WHERE
			);

			$shown[$type] = $hidden[$type] = 0;
			if (!count($objects)) {
				continue;
			}

			$dropCache = true;
			Label('Publishing '.$type.' objects');
			foreach ($objects as $v) {
				WorkProgress(false, count($objects));

				// disable triggering status change if there is no reason to do it in future and switch status
				PXDeferredStatusHelper::applyNextState($v);

				$db->ModifyContentObject($format, $v);

				// stats ^.^
				$v['status'] ? $shown[$type]++ : $hidden[$type]++;
			}
			PXTriggerDatabaseAdvert::finishMassADLoad();

			WorkProgress(true);
		}

		if ($dropCache) {
			$db->cacheOn();
			$db->clearCache();
		}

		$shown = json_encode($shown);
		$hidden = json_encode($hidden);
		$stats = "Shown: {$shown}, hidden: {$hidden}, cacheDropped: {$dropCache}";
		Label($stats);

		return array('status' => 1, 'note' => $stats);
	}

}
