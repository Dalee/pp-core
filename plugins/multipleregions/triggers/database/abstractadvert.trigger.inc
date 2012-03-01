<?php

require_once BASEPATH . '/libpp/triggers/database/abstractadvert.trigger.inc';

class PXTriggerDatabaseMultiRegionalAbstractAdvert extends PXTriggerDatabaseAbstractAdvert {
	// TODO write method for automatic adsync donor with clone
	// function onAddObject() {
	// }

	protected function getBannersById($id) {
		$regionsObj = PXMultiRegions::getInstance();
		$enabled = !$regionsObj->isDisabled();

		$enabled && $regionsObj->disable();
		$result = parent::getBannersById($id);
		$enabled && $regionsObj->enable();

		return $result;
	}

	function adSync($format, $object) {
		parent::adSync($format, $object);
		$regionsObj = PXMultiRegions::getInstance();
		if (
			$regionsObj->isRegionalObject($this->db->app->types[$format]) && 
			$data = $this->db->query("SELECT ".PXMultipleRegionsReflexer::REFLEX_FIELD." FROM {$format} WHERE id={$object['id']}")
		) {
			$sysReflexId = current(current($data));

			if ($sysReflexId == $object['id']) {
				return;
			}

			$enabled = !$regionsObj->isDisabled();
			$enabled && $regionsObj->disable();
			$reflex = $this->db->getObjectById($this->db->app->types[$format], $sysReflexId);
			$enabled && $regionsObj->enable();

			parent::adSync($format, $reflex);
		}
	}

}

?>
