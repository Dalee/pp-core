<?php

require_once BASEPATH . '/libpp/triggers/database/abstractadvert.trigger.inc';

class PXTriggerDatabaseMultiRegionalAbstractAdvert extends PXTriggerDatabaseAbstractAdvert {
	// TODO write method for automatic adsync donor with clone
	// function onAddObject() {
	// }

	protected function getBannersById($id) {
		$regionsObj = PXMultiRegions::getInstance();

		$regionsObj->disable();
		$result = parent::getBannersById($id);
		$regionsObj->enable();

		return $result;
	}
}

?>
