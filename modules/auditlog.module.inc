<?php

use PP\Module\ModuleAbstract;

/**
 * Класс модуля вывода Журнала Аудита.
 *
 * @author Andrey Nikolaev <na@index20.ru>
 */
class PXModuleAuditLog extends ModuleAbstract {
	function PXModuleAuditLog($area, $settings) {
		parent::__construct($area, $settings);
		if(is_callable(array($this->layout, 'setOneColumn'))){
			$this->layout->setOneColumn();
		}
	}

	function adminIndex() {
		require_once BASEPATH . '/libpp/lib/Logger/Audit/wrapper.class.inc';
		$audit_wrapper = new PXAdminAuditWrapper();
		$audit_wrapper->init_and_render();
	}
}
