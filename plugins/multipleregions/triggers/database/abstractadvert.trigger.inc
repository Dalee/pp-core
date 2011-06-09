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

	function adSync($format, $object) {
		parent::adSync($format, $object);
		if ($format == 'adbanner' && $data = $this->db->query("SELECT ".PXMultipleRegionsReflexer::REFLEX_FIELD." FROM {$format} WHERE id={$object['id']}")) {
			$sysReflexId = current(current($data));

			if ($sysReflexId == $object['id']) {
				return;
			}

			PXMultiRegions::getInstance()->disable();
			$reflex = $this->db->getObjectById($this->db->app->types[$format], $sysReflexId);
			PXMultiRegions::getInstance()->enable();

			parent::adSync($format, $reflex);
		}
	}

}

?>
