<?php

use PP\Plugin\AbstractPlugin;

class PXPluginPreview extends AbstractPlugin {
    public const MODULE_NAME = 'adminpreview';
    public const PREVIEW_ACL_TYPE = 'admin_preview';
    public const ENABLE_PREVIEW_FLAG = 'admin_preview';

	public function initialize($app) {
		$this->loadModule(self::MODULE_NAME);
		$this->loadTrigger('database/preview');
		$this->loadOnlyInAdmin('lib/widgets/preview.class.inc');
		$this->loadOnlyInAdmin('decorators.php');
	}
}
