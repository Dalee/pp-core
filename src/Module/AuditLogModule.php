<?php

namespace PP\Module;

/**
 * Class AuditLogModule.
 *
 * @package PP\Module
 */
class AuditLogModule extends AbstractModule
{
    public function __construct($area, $settings)
    {
        parent::__construct($area, $settings);

        if (is_callable([$this->layout, 'setOneColumn'])) {
            $this->layout->setOneColumn();
        }
    }

    public function adminIndex()
    {
        require_once PPCOREPATH . 'lib/Logger/Audit/wrapper.class.inc';

        $auditWrapper = new \PXAdminAuditWrapper();
        $auditWrapper->init_and_render();
    }

}
