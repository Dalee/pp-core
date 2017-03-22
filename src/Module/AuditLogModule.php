<?php

namespace PP\Module;

/**
 * Class AuditLogModule.
 *
 * @package PP\Module
 */
class AuditLogModule extends AbstractModule {

	public function __construct($area, $settings) {
		parent::__construct($area, $settings);

		if (is_callable(array($this->layout, 'setOneColumn'))){
			$this->layout->setOneColumn();
		}
	}

	function adminIndex() {
		require_once PPPATH . 'lib/Logger/Audit/wrapper.class.inc';

		$auditWrapper = new \PXAdminAuditWrapper();
		$auditWrapper->init_and_render();
	}

}
