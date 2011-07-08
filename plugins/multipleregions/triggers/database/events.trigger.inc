<?php

/*
* отложенные манипуляции с регионами
* просто добавляет в sys_regions_events запись
* все  действия осуществляются в cronruns
*/

class PXTriggerDatabaseEvents extends PXAbstractDatabaseTrigger {
	const TYPE = PXMultiRegions::REGION_MARK;

	function onCloneObject($db, $format, &$object, $donor) {
		if ($format->id != self::TYPE) return;

		if (is_callable(array('PXEventsQueue', 'enqueue'))) {
			PXEventsQueue::enqueue(array(
				'callback' => array('class' => 'PXSREventsCallback', 'method' => 'addToObjects'), 
				'params'   => array('from' => $donor['id'], 'to' => $object['id'])));
		}
	}
}

?>
