<?php
//sql view required:
//CREATE VIEW log_audit_view AS SELECT "id", "ts", to_date(ts::text, 'DD.MM.YYYY'::text) as date, "level", "type", "source", "user", "ip", "description", "diff" FROM log_audit;

/**
 * Класс модуля вывода Журнала Аудита.
 *
 * @author Andrey Nikolaev <na@index20.ru>
 */
class PXModuleAuditLog extends PXModule {
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
?>